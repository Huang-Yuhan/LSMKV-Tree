#include "kvstore.h"
#include <string>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <limits>

const bool MODE_TIERING=true;
const bool MODE_LEVELING=false;

void memcpyBoolToChar(char *_dst,bool *_src,int byte_size);
void memcpyChartoBool(bool *_dst,char *_src,int byte_size);
void split(const std::string &s,std::vector<std::string> &v,char delim);
bool cmp(BloomFilter*a,BloomFilter *b);


KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	timeStamp=0;
	std::vector<std::string> fileFolder;
	int fileFolderNum=utils::scanDir("data",fileFolder);

	uint64_t timeStampTmp=0;

	for(int i=0;i<fileFolderNum;i++)
	{
		std::vector<std::string> files;
		int fileNum=utils::scanDir("data/"+fileFolder[i],files);
		std::vector<BloomFilter*> sstablefiles;
		for(int j=0;j<fileNum;j++)
		{
			std::string filePath="data/"+fileFolder[i]+"/"+files[j];

			std::vector<std::string> fileSplit;
			split(files[j],fileSplit,'_');
			std::istringstream is(fileSplit[0]);
			is>>timeStampTmp;
			timeStamp=std::max(timeStamp,timeStampTmp);
			BloomFilter *cache=readSSTable(filePath);
			BloomFilters.push_back(cache);
			sstablefiles.push_back(cache);
		}
		std::sort(sstablefiles.begin(),sstablefiles.end(),cmp);
		SSTalbeFiles.push_back(sstablefiles);
	}

	readConfiguration();
}

KVStore::~KVStore()
{
	if(!utils::dirExists("data/level-0"))
	{
		if(utils::mkdir("data/level-0")!=0)
		{
			throw "mkdir error";
		}
	}
	MemToSS("data/level-0");
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	skipListData insertData(key,s);
	int NewMemSize=sizeof(uint64_t)+s.size()*sizeof(char)+sizeof(uint32_t);
	if(NewMemSize+memtable.totoalMemSize>memtable.MaxBytes)
	{
		if(!utils::dirExists("data/level-0"))
		{
			if(utils::mkdir("data/level-0")!=0)
			{
				throw "mkdir error";
			}
		}
		MemToSS("data/level-0");
		memtable.clear();
		if(SSTalbeFiles.empty())SSTalbeFiles.push_back(std::vector<BloomFilter*>(1,BloomFilters.back()));
		else SSTalbeFiles[0].push_back(BloomFilters.back());
		compaction(0);
	}
	memtable.insert(insertData);
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	std::string s;
	skipListData getData= memtable.search(key);
	if(getData.value=="~DELETED~")return "";
	if(getData==findError)				//not find in memtable 
	//ready search in the SSTable
	s= searchSSTable(key);
	else s= getData.value;
	return s;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	skipListData tmp=memtable.search(key);
	/*if(tmp==findError||tmp.value=="~DELETED~")return false;
	skipListData delData(key,"~DELETED~");
	memtable.insert(delData);*/
	if(tmp.value=="~DELETED~")return false;
	if(tmp==findError&&searchSSTable(key)=="")			//if not found
	return false;
	skipListData delData(key,"~DELETED~");
	memtable.insert(delData);
	return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	timeStamp=0;
	memtable.clear();
	for(auto it = BloomFilters.begin();it!=BloomFilters.end();it++)
	{
		delete (*it);
	}
	BloomFilters.clear();
	std::vector<std::string> fileFolder;
	int fileFolderNum=utils::scanDir("data",fileFolder);
	for(int i=0;i<fileFolderNum;i++)
	{
		std::vector<std::string> files;
		int fileNum=utils::scanDir("data/"+fileFolder[i],files);
		for(int j=0;j<fileNum;j++)
		{
			std::string filePath="data/"+fileFolder[i]+"/"+files[j];
			if(utils::rmfile(filePath.c_str()))
				throw "rm file "+files[j]+"error";
		}
	}

	for(int i=0;i<SSTalbeFiles.size();i++)
	{
		SSTalbeFiles[i].clear();
	}
	SSTalbeFiles.clear();
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list)
{	
	memtable.scan(key1,key2,list);
}


// convert the memtable to SStable
void KVStore::MemToSS(const std::string &dir)
{
	if(memtable.keyNum==0)return;
	std::string fileName=std::to_string(timeStamp)+"_"+std::to_string(memtable.keyNum)+"_";
	uint64_t headerOffset=0;
	uint64_t BFOffset=32;
	uint64_t indexOffset=10240+32;
	uint64_t indexLength=memtable.keyNum*(sizeof(uint64_t)+sizeof(uint32_t));
	uint64_t dataOffset=indexOffset+indexLength;
	std::ofstream os;
	bool bloomfilter[BloomFilterMaxSize];
	memset(bloomfilter,0,sizeof(bloomfilter));
	skipListNode *p=memtable.begin();
	unsigned int hash[4]={0};		
	uint64_t keymin,keymax;
	keymin=p->val.key;			

	char *buffer=new char[memtable.totoalMemSize];
	memcpy(buffer,&timeStamp,sizeof(uint64_t));							//Header---timeStamp
	memcpy(buffer+8,&memtable.keyNum,sizeof(uint64_t));					//Header---keyNum
	memcpy(buffer+16,&keymin,sizeof(uint64_t));							//Header---keymin
	int index;
	uint32_t valOffset,preOffset=dataOffset;

	std::vector<uint64_t> mem_key;
	std::vector<uint32_t> mem_offset;

	for(int pos=0;pos<memtable.keyNum;pos++)
	{
		index=indexOffset+pos*(sizeof(uint64_t)+sizeof(uint32_t));
		memcpy(buffer+index,&(p->val.key),sizeof(uint64_t));			//key
		mem_key.push_back(p->val.key);
		valOffset=preOffset;
		preOffset=valOffset+sizeof(char)*p->val.value.size();
		memcpy(buffer+index+sizeof(uint64_t),&valOffset,sizeof(uint32_t));		//offset
		mem_offset.push_back(valOffset);
		memcpy(buffer+valOffset,p->val.value.c_str(),sizeof(char)*p->val.value.size());	//value
		MurmurHash3_x64_128(&(p->val.key),sizeof(uint64_t),1,hash);
		for(int i=0;i<4;i++)bloomfilter[hash[i]%BloomFilterMaxSize]=true;
		keymax=p->val.key;
		p=p->next[0];
	}
	memcpy(buffer+24,&keymax,sizeof(uint64_t));							//Header---keymax
	memcpyBoolToChar(buffer+BFOffset/sizeof(char),bloomfilter,10240);							//BF
	fileName+=std::to_string(keymin)+"_"+std::to_string(keymax)+".sst";
	const std::string filePath=dir+"/"+fileName;
	os.open(filePath,std::ios::out|std::ios::binary);
	if(os.fail())
	{
		throw "create file error";
	}
	os.write(buffer,memtable.totoalMemSize);
	os.close();


	//store the bloomfilter in the cache

	BloomFilter * tmp=new BloomFilter(timeStamp,buffer+BFOffset/sizeof(char),mem_key,mem_offset,filePath);
	BloomFilters.push_back(tmp);
	timeStamp++;

	delete[] buffer;
}

BloomFilter::BloomFilter(uint64_t Stamp,char *s,const std::vector<uint64_t> &key,const std::vector<uint32_t> &offset,const std::string &path)
:cacheKey(key),cacheOffset(offset),filePath(path),BtimeStamp(Stamp)
{
	memcpyChartoBool(mark,s,10240);
	keymin=key[0];
	keymax=key[key.size()-1];
	keyNum=key.size();

}

bool BloomFilter::check(uint64_t key)
{
	memset(hash,0,sizeof(hash));
	MurmurHash3_x64_128(&key,sizeof(uint64_t),1,hash);
	for(int i=0;i<4;i++)if(!mark[hash[i]%81920])return false;
	return true;

}

uint32_t BloomFilter::getOffset(uint64_t key,uint32_t &length)
{
	std::vector<uint64_t>::iterator it=std::lower_bound(cacheKey.begin(),cacheKey.end(),key);	
	if(it==cacheKey.end()||(*it)!=key)return 0;
	int index=it-cacheKey.begin();
	if(index+1==cacheOffset.size())
	{
		length=0;
		return cacheOffset[index];
	}
	else
	{
		length=cacheOffset[index+1]-cacheOffset[index];
		return cacheOffset[index];
	}
}

std::string BloomFilter::getFilePath() const
{return filePath;}

std::string KVStore::getFileData(const std::string &filePath,uint32_t offset,uint32_t length)
{
	std::ifstream fin;
	fin.open(filePath,std::ios::in|std::ios::binary);
	if(fin.fail())
	{
		throw "Open SStable error";
	}
	if(!length)
	{
		fin.seekg(offset,std::ios::beg);
		auto beginpos=fin.tellg();
		fin.seekg(0,std::ios::end);
		auto endpos=fin.tellg();
		length=endpos-beginpos;
	}
	if(!fin.seekg(offset,std::ios::beg))
	{
		throw "read SStable error";
	}else
	{
		char *s=new char[length+1];
		s[length]='\0';
		fin.read(s,length*sizeof(char));
		std::string str(s);
		delete[] s;
		return str;
	}
}

std::string KVStore::searchSSTable(uint64_t key)
{
	std::string ans="";
	int dataTimeStamp=0;
	for(auto it=BloomFilters.rbegin();it!=BloomFilters.rend();it++)
	{
		if(*it==nullptr)continue;
		if(dataTimeStamp>(*it)->getTimeStamp())continue;
		if(key<(*it)->getKeyMin()||(*it)->getKeyMax()<key)continue;;
		if((*it)->check(key))
		{
			uint32_t length=0;
			int offset=(*it)->getOffset(key,length);
			if(offset)
			{
				const std::string s=getFileData((*it)->getFilePath(),offset,length);
				ans=s;
				dataTimeStamp=(*it)->getTimeStamp();
			}
		}
	}
	if(ans=="~DELETED~")ans="";
	return ans;
}

uint64_t BloomFilter::getKeyMax()const
{return keymax;}

uint64_t BloomFilter::getKeyMin()const
{return keymin;}

void memcpyBoolToChar(char *_dst,bool *_src,int byte_size)
{
	char *dst=_dst;
	bool *src=_src;
	for(int i=0;i<byte_size;i++)
	{
		char sum=0;
		for(int j=0;j<8;j++)
		sum=sum<<1|src[j];
		src+=8;
		*dst=sum;
		dst++;
	}
}	

void memcpyChartoBool(bool *_dst,char *_src,int byte_size)
{
	bool *dst=_dst;
	char *src=_src;
	for(int i=0;i<byte_size;i++)
	{
		char tmp=*(src+i);
		for(int j=7;j>=0;j--)
		{
			dst[j]=tmp&1;
			tmp>>=1;
		}
		dst+=8;
	}
}

/*
TODO:
*/

BloomFilter* KVStore::readSSTable(const std::string &filePath)
{
	std::ifstream fin;	
	fin.open(filePath,std::ios::in|std::ios::binary);
	std::vector<std::string> fileSplit;
	split(filePath,fileSplit,'/');
	std::string fileName=fileSplit.back();
	fileSplit.clear();
	split(fileName,fileSplit,'_');
	std::istringstream is(fileSplit[1]+" "+fileSplit[0]);
	uint64_t SSTableTimeStamp;
	uint64_t keyNum;
	uint64_t keyMin,keyMax;

	//BloomFilter::BloomFilter(char *s,const std::vector<uint64_t> &key,const std::vector<uint32_t> &offset,const std::string &path)

	char *buffer;

	is>>keyNum>>SSTableTimeStamp;

	int bufferLength=32+10240;

	buffer=new char[bufferLength];
	fin.read(buffer,bufferLength);

	std::ofstream fout("err/level-0/"+fileName);
	#pragma pack(4)
	struct KeyOffset
	{
		uint64_t key;
		uint32_t offset;
	}*KO;
	#pragma pack()
	KO=new KeyOffset[keyNum];
	fin.read((char*)KO,keyNum*(sizeof(uint64_t)+sizeof(uint32_t)));
	std::vector<uint64_t> key;
	std::vector<uint32_t> offset;
	for(int i=0;i<keyNum;i++)		//has some problem ?
	{
		key.push_back(KO[i].key);
		offset.push_back(KO[i].offset);
//		fout<<KO[i].key<<" "<<KO[i].offset<<"\n";
	}
	BloomFilter *tmp=new BloomFilter(SSTableTimeStamp,&buffer[32],key,offset,filePath);
	delete[] KO;
	delete[] buffer;
	fin.close();
	fout.close();
	return tmp;
}

void split(const std::string &s,std::vector<std::string> &v,char delim)
{
	int beginpos=0;
	for(int i=0;i<s.size();i++)
	{
		if(s[i]==delim)
		{
			v.push_back(s.substr(beginpos,i-beginpos));
			beginpos=i+1;
		}
	}
	v.push_back(s.substr(beginpos,s.size()-beginpos));
}

void KVStore::compaction(int level)
{
	while (level+1>=levelConfiguration.size())levelConfiguration.push_back(std::make_pair(levelConfiguration.back().first*2,"Leveling"));

	if(SSTalbeFiles[level].size()<=levelConfiguration[level].first)return;

	int fileLevelXNumMax=levelConfiguration[level].first;
	bool fileMode=levelConfiguration[level].second=="Tiering";

	std::vector<BloomFilter*> selected;
	//Select files
	selectXPlus(level+1,fileMode,selected);
	selectX(level,fileMode,selected);

	mergeSort(selected,level+1);

	compaction(level+1);

}

void KVStore::readConfiguration()
{
	std::ifstream fin;
	fin.open("default.conf");
	int level,fileMaxNum;
	std::string fileMode;
	while(fin>>level>>fileMaxNum>>fileMode)
	{
		levelConfiguration.push_back(std::make_pair(fileMaxNum,fileMode));
	}
}

uint64_t BloomFilter::getKeyNum()const
{
	return keyNum;
}

uint64_t BloomFilter::getTimeStamp()const
{
	return BtimeStamp;
}

bool cmp(BloomFilter *a,BloomFilter *b)
{
	if(a->getTimeStamp()!=b->getTimeStamp())return a->getTimeStamp()<b->getTimeStamp();
	if(a->getKeyMin()!=b->getKeyMin())return a->getKeyMin()<b->getKeyMin();
	return a->getKeyMax()<b->getKeyMax();
}

void KVStore::selectX(int level, bool mode, std::vector<BloomFilter *> &selected)
{
	std::vector<BloomFilter *> &files=SSTalbeFiles[level];
	if(mode==MODE_TIERING)// select all files
	{
		for(int i=0;i<files.size();i++)
		{
			selected.push_back(files[i]);
		}
		files.clear();
	}
	else
	{
		std::sort(files.begin(),files.end(),cmp);
		int endpos=files.size()-levelConfiguration[level].first;
		for(int i=0;i<endpos;i++)selected.push_back(files[i]);
		files.erase(files.begin(),files.begin()+endpos);
	}
}

void KVStore::selectXPlus(int level, bool mode, std::vector<BloomFilter *> &selected)
{
	if(SSTalbeFiles.size()<=level)return;
	std::vector<BloomFilter *> &files=SSTalbeFiles[level];
	std::vector<BloomFilter *> &preFiles=SSTalbeFiles[level-1];
	if(mode==MODE_TIERING)// select all files
	{
		return;
	}
	else
	{
		uint64_t keymin,keymax;
		keymin=preFiles[0]->getKeyMin();
		keymax=preFiles[0]->getKeyMax();
		for(int i=1;i<preFiles.size();i++)
		{
			keymin=std::min(keymin,preFiles[i]->getKeyMin());
			keymax=std::max(keymax,preFiles[i]->getKeyMax());
		}

		for(auto it=files.begin();it!=files.end();)
		{
			if(
				keymin<=(*it)->getKeyMin()&&(*it)->getKeyMin()<=keymax ||
				keymin<=(*it)->getKeyMax()&&(*it)->getKeyMax()<=keymax 
			)
			{
				selected.push_back(*it);
				it=files.erase(it);
			}
			else it++;
		}
	}
}

struct mergeSortFile
{
	uint64_t key_num=0;
	uint64_t time_stamp=0;
	uint64_t key_min=0;
	uint64_t key_max=0;
	uint64_t totalMemSize=10240+32;
	std::vector<uint64_t> keys;
	std::vector<std::string> values;
	std::vector<uint32_t> offsets;

	BloomFilter * write_to_file(std::string filePath);
	void read_from_file(std::string &filePath);		//TODO: change it read from cache(Bloomfileter)
	void read_from_blom(BloomFilter * obj);
	void add(uint64_t key,uint64_t offset,std::string &value);
	void init();
};

void mergeSortFile::read_from_blom(BloomFilter * obj)
{
	std::ifstream fin;
	fin.open(obj->getFilePath(),std::ios::in|std::ios::binary);
	
	// copy the file content to buffer
	
	auto beginpos=fin.tellg();
	fin.seekg(0,std::ios::end);
	auto endpos=fin.tellg();
	auto file_length=endpos-beginpos;
	fin.seekg(0,std::ios::beg);

	if(fin.fail())
	{
		throw "open file "+obj->getFilePath()+" in read_from_file error";
	}

	char *buffer=new char[file_length+1];
	buffer[file_length]='\0';
	fin.read(buffer,file_length);

	fin.close();

	//copy from obj

	time_stamp=obj->getTimeStamp();
	key_num=obj->getKeyNum();
	key_min=obj->getKeyMin();
	key_max=obj->getKeyMax();

	// read data from buffer
	int beg=10240+32;
	totalMemSize=beg;
	beg+=key_num*(sizeof(uint64_t)+sizeof(uint32_t));
	char * pos=&buffer[beg];
	char *str_buffer=new char[file_length+1];
	offsets=obj->cacheOffset;
	keys=obj->cacheKey;
	// value start

	for(uint64_t i=0;i<key_num;i++)
	{
		uint32_t value_length=file_length;
		if(i+1<key_num)value_length=offsets[i+1];
		value_length-=offsets[i];
		memcpy(str_buffer,pos,value_length);
		pos+=value_length;
		values.push_back(std::string(str_buffer,value_length));
		totalMemSize+=sizeof(uint64_t)+sizeof(uint32_t)+value_length;
	}

	utils::rmfile(obj->getFilePath().c_str());
}

BloomFilter * mergeSortFile::write_to_file(std::string dir)
{
	std::string fileName=std::to_string(time_stamp)+"_"+std::to_string(key_num)+"_";
	fileName+=std::to_string(key_min)+"_"+std::to_string(key_max)+".sst";
	uint64_t headerOffset=0;
	uint64_t BFOffset=32;
	uint64_t indexOffset=10240+32;
	uint64_t indexLength=key_num*(sizeof(uint64_t)+sizeof(uint32_t));
	uint64_t dataOffset=indexOffset+indexLength;
	std::ofstream os;
	bool bloomfilter[81920];
	memset(bloomfilter,0,sizeof(bloomfilter));
	unsigned int hash[4]={0};		
	char *buffer=new char[totalMemSize];

	memcpy(buffer,&time_stamp,sizeof(uint64_t));
	memcpy(buffer+8,&key_num,sizeof(uint64_t));
	memcpy(buffer+16,&key_min,sizeof(uint64_t));
	memcpy(buffer+24,&key_max,sizeof(uint64_t));

	uint32_t key_offset,val_offset,pre_offset;
	pre_offset=dataOffset;

	for(int i=0;i<key_num;i++)
	{
		//copy key
		key_offset=indexOffset+i*(sizeof(uint64_t)+sizeof(uint32_t));
		memcpy(buffer+key_offset,&keys[i],sizeof(uint64_t));
		//copy value & offset
		val_offset=pre_offset;
		pre_offset=val_offset+values[i].size();
		memcpy(buffer+key_offset+sizeof(uint64_t),&val_offset,sizeof(uint32_t));
		memcpy(buffer+val_offset,values[i].c_str(),values[i].size());
		//hash
		MurmurHash3_x64_128(&keys[i],sizeof(uint64_t),1,hash);
		for(int i=0;i<4;i++)bloomfilter[hash[i]%81920]=true;
	}

	//copy blommfilter into buffer

	memcpyBoolToChar(buffer+32,bloomfilter,10240);

	const std::string filePath=dir+"/"+fileName;
	os.open(filePath,std::ios::out|std::ios::binary);
	if(os.fail())
	{
		throw "create file error";
	}
	os.write(buffer,totalMemSize);
	os.close();
	
	BloomFilter * tmp= new BloomFilter(time_stamp,buffer+32,keys,offsets,filePath);

	delete[] buffer;

	return tmp;
}

void mergeSortFile::read_from_file(std::string &filePath)
{
	std::ifstream fin;
	fin.open(filePath,std::ios::in|std::ios::binary);
	
	// copy the file content to buffer
	
	auto beginpos=fin.tellg();
	fin.seekg(0,std::ios::end);
	auto endpos=fin.tellg();
	auto file_length=endpos-beginpos;
	fin.seekg(0,std::ios::beg);
	totalMemSize=file_length;
	if(fin.fail())
	{
		throw "open file "+filePath+" in read_from_file error";
	}

	char *buffer=new char[file_length+1];
	buffer[file_length]='\0';
	fin.read(buffer,file_length);

	fin.close();

	// read data from buffer

	char * pos=buffer;
	char *str_buffer=new char[file_length+1];

	memcpy(pos,&time_stamp,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(pos,&key_num,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(pos,&key_min,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(pos,&key_max,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	pos=&buffer[10240+32];		//key start
	uint32_t offset_tmp;
	uint64_t key_tmp;
	for(uint64_t i=1;i<=key_num;i++)
	{
		memcpy(&key_tmp,pos,sizeof(uint64_t));
		pos+=sizeof(uint64_t);

		memcpy(&offset_tmp,pos,sizeof(uint32_t));
		pos+=sizeof(uint32_t);

		keys.push_back(key_tmp);
		offsets.push_back(offset_tmp);
	}

	// value start

	for(uint64_t i=0;i<key_num;i++)
	{
		uint32_t value_length=file_length;
		if(i+1<key_num)value_length=offsets[i+1];
		value_length-=offsets[i];
		memcpy(str_buffer,pos,value_length);
		pos+=value_length;
		values.push_back(std::string(str_buffer,value_length));
	}
}

struct mergeNode
{
	mergeSortFile *pointer;
	int index;
	mergeNode(mergeSortFile*p,int _index):pointer(p),index(_index){}
	bool operator<(const mergeNode &obj)const
	{
		return pointer->keys[index]<=obj.pointer->keys[obj.index];
	}
};

void mergeSortFile::add(uint64_t key,uint64_t offset,std::string &value)
{
	keys.push_back(key);
	offsets.push_back(offset);
	values.push_back(value);
	totalMemSize+=sizeof(uint64_t)+sizeof(uint32_t)+value.size();
}
void mergeSortFile::init()
{
	key_num=keys.size();
	uint64_t Min=std::numeric_limits<uint64_t>::max();
	uint64_t Max=0;
	for(int i=0;i<key_num;i++)
	{
		Min=std::min(Min,keys[i]);
		Max=std::max(Max,keys[i]);
	}
	key_min=Min;
	key_max=Max;
}

void KVStore::mergeSort(std::vector<BloomFilter*> &selectd,int level)
{
	//BloomFilters need delete the one in selected
	std::sort(BloomFilters.begin(),BloomFilters.end(),cmp);

	std::vector<mergeSortFile*> files;
	std::vector<mergeNode> sortArray;
	for(int i=0;i<selectd.size();i++)
	{
		auto it=lower_bound(BloomFilters.begin(),BloomFilters.end(),selectd[i],cmp);
		BloomFilters.erase(it);

		mergeSortFile * ptr=new mergeSortFile();

		files.push_back(ptr);
		ptr->read_from_blom(selectd[i]);
		for(int j=ptr->key_num-1;j>=0;j--)
		{
			sortArray.push_back(mergeNode(ptr,j));
		}
	}

	std::sort(sortArray.begin(),sortArray.end());

	// up to now the sstable in selectd have cleared their cache in KVStore(SSTalbeFiles & BloomFilters)

	int idx=0;
	uint64_t time_stamp=0,time_stamp_tmp,idx_tmp;

	std::string dir="data/level-"+std::to_string(level);
	if(!utils::dirExists(dir))
	{
		if(utils::mkdir(dir.c_str())!=0)
		{
			throw "mkdir error";
		}
	}

	mergeSortFile * filetmp=new mergeSortFile(),*InSortArrty;
	for(idx;idx<sortArray.size();idx++)
	{
		idx_tmp=0;
		time_stamp_tmp=0;
		while(idx+1<sortArray.size()
			&& sortArray[idx].pointer->keys[sortArray[idx].index] == sortArray[idx+1].pointer->keys[sortArray[idx+1].index]
		)
		{
			if(time_stamp_tmp<=sortArray[idx].pointer->time_stamp)
			{
				idx_tmp=idx;
				time_stamp_tmp=sortArray[idx].pointer->time_stamp;
			}
			idx++;
		}

		if(time_stamp_tmp<=sortArray[idx].pointer->time_stamp)
		{
			idx_tmp=idx;
			time_stamp_tmp=sortArray[idx].pointer->time_stamp;
		}
		//在相同key中选出time_stamp最大的
		InSortArrty=sortArray[idx_tmp].pointer;
		idx_tmp=sortArray[idx_tmp].index;
		if(InSortArrty->values[idx_tmp]!="~DELETED~")
		{
			//如果即将超过2MB
			if(filetmp->totalMemSize+sizeof(uint64_t)+sizeof(uint32_t)+InSortArrty->values[idx_tmp].size()>2097152)
			{
				filetmp->init();
				BloomFilters.push_back(filetmp->write_to_file(dir));
				if(SSTalbeFiles.size()<=level)SSTalbeFiles.push_back(std::vector<BloomFilter*>());
				SSTalbeFiles[level].push_back(BloomFilters.back());
				delete filetmp;
				filetmp= new mergeSortFile();
			}
			filetmp->add(InSortArrty->keys[idx_tmp],
			InSortArrty->offsets[idx_tmp],
			InSortArrty->values[idx_tmp]
			);
			filetmp->time_stamp=std::max(filetmp->time_stamp,time_stamp_tmp);
		}
	}
	filetmp->init();
	BloomFilters.push_back(filetmp->write_to_file("data/level-"+std::to_string(level)));
	delete filetmp;
	for(int i=0;i<files.size();i++)delete files[i];

}