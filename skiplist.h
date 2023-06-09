//
// Created by Huang_Yuhan on 2023/4/4.
//

#ifndef LSMKV_TREE_SKIPLIST_H
#define LSMKV_TREE_SKIPLIST_H
#include<limits>
#include<vector>
#include<string>
#include<list>
#include<fstream>
class skipListData
{
public:
    uint64_t key;
    std::string value;
    skipListData(){}
    skipListData(uint64_t _key,const std::string &_value):key(_key),value(_value)
    {}
    bool operator<(const skipListData&obj)const
    {
        return key<obj.key;
    }
    bool operator==(const skipListData&obj)const
    {
        return key==obj.key&&value==obj.value;
    }
    bool operator!=(const skipListData&obj)const
    {
        return key!=obj.key||value!=obj.value;
    }
};
class skipListNode
{
public:
    std::vector<skipListNode*> next;
    skipListData val;
    skipListNode(skipListData v):val(v)
    {
        next.resize(32, nullptr);
    }
};

extern skipListData findError;

class SkipList {
private:
    skipListNode *head,*tail;
    double pr;                  //概率
    static const int MaxLevel=32;
    inline uint64_t getStrMemSize(const std::string& s);
public:
    static const int MaxBytes=2*1024*1024;
    uint64_t totoalMemSize;
    uint64_t keyNum;                 //numbers of keys
    void insert(skipListData &x);
    skipListData& search(uint64_t key);
    SkipList();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t, std::string> > &list);
    skipListNode*const begin();
    skipListNode*const end();
    void clear();
};

#endif //LSMKV_TREE_SKIPLIST_H
