#pragma once

#include "kvstore_api.h"
#include "skiplist.h"
#include<fstream>
#include "MurmurHash3.h"
#include<cstring>
#include<vector>
#include"utils.h"

class BloomFilter
{
	private:
		bool mark[81920];				//Bloom Filter
		std::vector<uint64_t> cacheKey;
		std::vector<uint32_t> cacheOffset;
		unsigned int hash[4];
		std::string filePath;			
	public:
		BloomFilter(const char *s,const std::vector<uint64_t> &key,const std::vector<uint32_t> &offset,const std::string &name);
		uint32_t getOffset(uint64_t key,uint32_t &length);							//return 0 if not find else offset, set the data length
		bool check(uint64_t key);
		const std::string getFilePath()const;
};

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	static const int BloomFilterMaxSize=81920;
	SkipList memtable;
	uint64_t timeStamp;				
	std::vector<BloomFilter*> BloomFilters;

	void MemToSS(std::string dir);
	std::string getFileData(const std::string &filePath,uint32_t offset,uint32_t length);
	std::string searchSSTable(uint64_t key);
public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list) override;
};
