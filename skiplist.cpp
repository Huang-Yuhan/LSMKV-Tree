//
// Created by Huang_Yuhan on 2023/4/4.
//
#include "skiplist.h"
#include <cstdlib>
#include<ctime>
#include<cmath>
#include<iostream>
SkipList::SkipList()
{
    srand(time(0));
    tail=new Skiplistnode(std::numeric_limits<int>::max());
    head=new Skiplistnode(std::numeric_limits<int>::min());
    for(int i=0;i<32;i++)head->next[i]=tail;
    SearchLength=0;
    pr=0.5;
}

void SkipList::insert(int x)
{
    //if(x%10000==0)std::cout<<"insert"<<x<<"\n";
    Skiplistnode *p=head;
    std::vector<Skiplistnode *> pre(MaxLevel);
    for(int i=MaxLevel-1;i>=0;i--)
    {
        while(p->next[i]!= tail&&p->next[i]->val<x)p=p->next[i];
        pre[i]=p;
    }
    p=new Skiplistnode(x);
    for(int i=0;i<MaxLevel;i++)
    {
        p->next[i]=pre[i]->next[i];
        pre[i]->next[i]=p;
        if(rand()&1)break;
    }
}

bool SkipList::search(int x)
{
    int height=32;
    Skiplistnode *p=head;
    std::vector<Skiplistnode *> pre(height);
    for(int i=height-1;i>=0;i--)
    {
        while(p->next[i]!= tail&&p->next[i]->val<x)p=p->next[i];
        pre[i]=p;
    }
    p=pre[0]->next[0];
    return p==tail && p->val==x;
}

int SkipList::GetSearchLength()
{
    return SearchLength;
}