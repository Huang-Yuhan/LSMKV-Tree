#include "kvstore.h"
#include <string>

KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	timeStamp=0;
}

KVStore::~KVStore()
{
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
			if(!utils::mkdir("data/level-0"))
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
	skipListData getData= memtable.search(key);
	if(getData==findError||getData.value=="~DELETED~")return "";
	else return getData.value;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	skipListData tmp=memtable.search(key);
	if(tmp==findError||tmp.value=="~DELETED~")return false;
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
void KVStore::MemToSS(std::string dir)
{
	std::string filePath=std::to_string(timeStamp)+"_"+std::to_string(memtable.keyNum)+"_";
	uint64_t headerOffset=0;
	uint64_t BFOffset=32;
	uint64_t indexOffset=10240+32;
	uint64_t indexLength=memtable.keyNum*(sizeof(uint64_t)+sizeof(uint32_t));
	uint64_t dataOffset=indexOffset+indexLength;
	std::ofstream os;
	bool bloomfilter[BloomFilterMaxSize];
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
	for(int pos=0;pos<memtable.keyNum;pos++)
	{
		index=indexOffset+pos*(sizeof(uint64_t)+sizeof(uint32_t));
		memcpy(buffer+index,&(p->val.key),sizeof(uint64_t));			//key
		valOffset=preOffset;
		preOffset=valOffset+sizeof(char)*p->val.value.size();
		memcpy(buffer+index+sizeof(uint64_t),&valOffset,sizeof(uint32_t));		//offset
		memcpy(buffer+valOffset,p->val.value.c_str(),sizeof(char)*p->val.value.size());	//value
		MurmurHash3_x64_128(&(p->val.key),sizeof(uint64_t),1,hash);
		for(int i=0;i<4;i++)bloomfilter[hash[i]%BloomFilterMaxSize]=true;
		keymax=p->val.key;
		p=p->next[0];
	}
	memcpy(buffer+24,&keymax,sizeof(uint64_t));							//Header---keymax
	memcpy(buffer+BFOffset,&bloomfilter,10240);							//BF
	filePath+=std::to_string(keymin)+"_"+std::to_string(keymax)+".ss";
	os.open(dir+"/"+filePath,std::ios::out|std::ios::binary);
	os.write(buffer,memtable.totoalMemSize);
	delete buffer;
	os.close();


	//store the bloomfilter in the cache

	BloomFilter * tmp=new BloomFilter((char*)bloomfilter);
	index=timeStamp-1;
	while(BloomFilters.size()<index)BloomFilters.push_back(nullptr);
	BloomFilters[index]=tmp;
}

BloomFilter::BloomFilter(const char *s)
{
	memcpy(mark,s,10240);
}

bool BloomFilter::check(uint64_t key)
{
	memset(hash,0,sizeof(hash));
	MurmurHash3_x64_128(&key,sizeof(uint64_t),1,hash);
	for(int i=0;i<4;i++)if(!mark[hash[i]%81920])return false;
	return true;

}