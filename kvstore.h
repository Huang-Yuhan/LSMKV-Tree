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
		bool mark[81920];
		unsigned int hash[4];
	public:
		BloomFilter(const char *s);
		bool check(uint64_t key);
};

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	static const int BloomFilterMaxSize=81920;
	SkipList memtable;
	uint64_t timeStamp;				
	void MemToSS();
	std::vector<BloomFilter*> BloomFilters;
public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list) override;
};
