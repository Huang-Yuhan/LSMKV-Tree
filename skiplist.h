//
// Created by Huang_Yuhan on 2023/4/4.
//

#ifndef LSMKV_TREE_SKIPLIST_H
#define LSMKV_TREE_SKIPLIST_H
#include<limits>
#include<vector>
class Skiplistnode
{
public:
    std::vector<Skiplistnode*> next;
    int val;
    Skiplistnode(int v):val(v)
    {
        next.resize(32, nullptr);
    }
};

class SkipList {
private:
    Skiplistnode *head,*tail;
    double pr;                  //概率
    int SearchLength;
    static const int MaxLevel=32;
public:
    void insert(int x);
    bool search(int x);
    int GetSearchLength();
    SkipList();
};
#endif //LSMKV_TREE_SKIPLIST_H
