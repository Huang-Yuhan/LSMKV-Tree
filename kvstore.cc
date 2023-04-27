#include "kvstore.h"
#include <string>
#include <algorithm>
#include <assert.h>
#include <sstream>

void memcpyBoolToChar(char *_dst,bool *_src,int byte_size);
void memcpyChartoBool(bool *_dst,char *_src,int byte_size);
void split(const std::string &s,std::vector<std::string> &v,char delim);

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
		for(int j=0;j<fileNum;j++)
		{
			std::string filePath="data/"+fileFolder[i]+"/"+files[j];

			std::vector<std::string> fileSplit;
			split(files[j],fileSplit,'_');
			std::istringstream is(fileSplit[0]);
			is>>timeStampTmp;
			timeStamp=std::max(timeStamp,timeStampTmp);
			if(filePath=="data/level-0/258_16881_0_32767.sst")
			{
				int x=1;
			}
			BloomFilter *cache=readSSTable(filePath);
			BloomFilters.push_back(cache);
		}
	}
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
	//check if the compaction needed
	while(level>=levelConfiguration.size())levelConfiguration.push_back(std::make_pair(levelConfiguration.back().first*2,"Leveling"));
	int fileMaxNum=levelConfiguration[level].first;
	std::string &fileMode=levelConfiguration[level].second;

	std::string dir="data/level-"+std::to_string(level);
	std::vector<std::string> files;
	utils::scanDir(dir,files);
	if(files.size()<=fileMaxNum)return;				//do not need to compaction

	//Select SSTable
	std::vector<std::string> seleceXFile,selectXPlusFile;
	selectX(level,seleceXFile);
	selectXPlus(level+1,selectXPlusFile);

	// SSTable merge
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

void KVStore::selectX(int level,std::vector<std::string> &selectFile)
{

}

void KVStore::selectXPlus(int level,std::vector<std::string> &selectFile)
{
	
}

uint64_t BloomFilter::getKeyNum()const
{
	return keyNum;
}

uint64_t BloomFilter::getTimeStamp()const
{
	return BtimeStamp;
}