//
// Created by Huang_Yuhan on 2023/4/4.
//
#include "skiplist.h"
#include <cstdlib>
#include<ctime>
#include<cmath>
#include<iostream>
#include "utils.h"

skipListData findError=skipListData(0,"\1");

SkipList::SkipList()
{
    keyNum=0;
    totoalMemSize=32+10240;
    srand(time(0));
    tail=new skipListNode(skipListData(0,""));
    head=new skipListNode(skipListData(0,""));
    for(int i=0;i<32;i++)head->next[i]=tail;
    pr=0.5;
    

}

void SkipList::insert(skipListData &x)
{
    //if(x%10000==0)std::cout<<"insert"<<x<<"\n";
    skipListData &searchResult= search(x.key);
    if(searchResult!=findError)
    {
        totoalMemSize=totoalMemSize+getStrMemSize(x.value)-getStrMemSize(searchResult.value);
        searchResult.value=x.value;
        return;
    }
    // insert a new (K,V) we need key(uint64) & offset(uint32) & value(calculate)
    totoalMemSize+=getStrMemSize(x.value);
    totoalMemSize+=sizeof(uint64_t)+sizeof(uint32_t);
    keyNum++;
    skipListNode *p=head;
    std::vector<skipListNode *> pre(MaxLevel);
    for(int i=MaxLevel-1;i>=0;i--)
    {
        while(p->next[i]!= tail&&p->next[i]->val<x)p=p->next[i];
        pre[i]=p;
    }
    p=new skipListNode(x);
    for(int i=0;i<MaxLevel;i++)
    {
        p->next[i]=pre[i]->next[i];
        pre[i]->next[i]=p;
        if(rand()&1)break;
    }
}

skipListData& SkipList::search(uint64_t key)
{
    int height=32;
    skipListNode *p=head;
    std::vector<skipListNode *> pre(height);
    for(int i=height-1;i>=0;i--)
    {
        while(p->next[i]!= tail&&p->next[i]->val.key<key)p=p->next[i];
        pre[i]=p;
    }
    p=pre[0]->next[0];
    if(p==tail||p->val.key!=key)return findError;
    return p->val;
}

void SkipList::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t, std::string> > &list)
{
    int height=32;
    skipListNode *p=head;
    std::vector<skipListNode *> pre(height);
    for(int i=height-1;i>=0;i--)
    {
        while(p->next[i]!= tail&&p->next[i]->val.key<key1)p=p->next[i];
        pre[i]=p;
    }
    p=pre[0]->next[0];
    while(p!=tail&&p->val.key<=key2)
    {
        list.push_back(std::make_pair(p->val.key,p->val.value));
        p=p->next[0];
    }
    return;
}

inline uint64_t SkipList::getStrMemSize(const std::string& s)
{
    return s.size()*sizeof(char);
}

skipListNode*const SkipList::begin()
{
    return head->next[0];
}
skipListNode*const SkipList::end()
{
    return tail;
}

void SkipList::clear()
{
    keyNum=0;
    totoalMemSize=32+10240;
    srand(time(0));
    skipListNode *p=head->next[0];
    while(p!=tail)
    {
        head->next[0]=p->next[0];
        delete p;
        p=head->next[0];
    }
    for(int i=0;i<32;i++)head->next[i]=tail;
    pr=0.5;
}