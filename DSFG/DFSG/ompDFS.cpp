#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <omp.h>
#include <chrono>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
using namespace std;

#define NUM_THREADS 4
//#define DFSGuseOMP
#define DFSGusePOINTER
int num_thread=2;
string res;
int resnums=1;
omp_lock_t lock;
struct node
{
    vector<int>list;
#ifdef DFSGusePOINTER
    int pointer=0;
#endif
    bool flag=false;//是否已访问
    bool exist=false;
};
vector<node>adjlist;//邻接表
long long adjStart[NUM_THREADS];
long long adjSize[NUM_THREADS];

void DFS(int start)
{
    printf("---begin DFS---\n");
    adjlist[start].flag=true;
    //printf("%4d\t",start);
    res+=to_string(start);
    stack<int>sck;
    sck.push(start);
    while(!sck.empty())
    {
        int now=sck.top();
        //printf("now is node %d\n",now);
        bool f=false;//如果从此点出发的边都已走过,则出栈
        // if (adjlist[now].list.size()==0)
        // {
        //     sck.pop();
        //     continue;
        // }
#ifndef DFSGuseOMP
        //use serial
        int p;
    #ifdef DFSGusePOINTER
        p=adjlist[now].pointer;
    #else
        p=0;
    #endif
        for(int i=p;i<adjlist[now].list.size();++i)//size()如果每次都要遍历得到长度的话，可以考虑手动换掉
        {
            if(!adjlist[adjlist[now].list[i]].flag)
            {
    #ifdef DFSGusePOINTER
                adjlist[now].pointer=i;
    #endif
                //adjlist[now].pointer=i;
                adjlist[adjlist[now].list[i]].flag=true;
                sck.push(adjlist[now].list[i]);
                //printf("%4d\t",adjlist[now].list[i]);
                //res.push_back(adjlist[now].list[i]);
                res+="\n"+to_string(adjlist[now].list[i]);
                resnums++;
                f=1;
                break;
            }
        }
        if(!f)
        {sck.pop();}
#else
        //parall
        int p;
    #ifdef DFSGusePOINTER
        p=adjlist[now].pointer;
    #else
        p=0;
    #endif
        if(adjlist[now].list.size()==0)
        {sck.pop();continue;}
        int nodes_per_thread=(adjlist[now].list.size()-1-p)/num_thread+1;
        //cout<<adjlist[now].list.size()<<" "<<nodes_per_thread<<endl;//
        bool thread_find[num_thread]={false};
        int thread_find_ele[num_thread]={0};
        #pragma omp parallel for schedule(static,nodes_per_thread) num_threads(num_thread)
        for(int i=p;i<adjlist[now].list.size();++i)//size()如果每次都要遍历得到长度的话，可以考虑手动换掉
        {
            int thread_num=omp_get_thread_num();
            if(!thread_find[thread_num]&&!adjlist[adjlist[now].list[i]].flag)
            {
                thread_find[thread_num]=true;
                thread_find_ele[thread_num]=i;
                i=adjlist[now].list.size();
            }
        }
        for(int i=0;i<num_thread;++i)
        {
            if(thread_find[i])
            {
    #ifdef DFSGusePOINTER
                adjlist[now].pointer=thread_find_ele[i];
    #endif
                int dis=adjlist[now].list[thread_find_ele[i]];
                adjlist[dis].flag=true;
                sck.push(dis);
                //printf("%4d\t",adjlist[now].list[i]);
                res+="\n"+to_string(dis);
                resnums++;
                f=1;
                break;
            }
        }
        // if(!adjlist[adjlist[now].list[i]].flag)
        // {
        //     adjlist[adjlist[now].list[i]].flag=true;
        //     sck.push(adjlist[now].list[i]);
        //     //printf("%4d\t",adjlist[now].list[i]);
        //     res.push_back(adjlist[now].list[i]);
        //     f=1;
        //     break;
        // }
        if(!f)
        {sck.pop();}
#endif
    }
}

int main(int argc,char* argv[])
{
    double ot1=omp_get_wtime();
    //adjlist init
    int n=atoi(argv[3]),m=atoi(argv[4]);
    for(int i=0;i<n;++i)
    {node a;adjlist.push_back(a);}
    //cout<<"some ok"<<endl;//
    //mmap init
    char *input=NULL;
    int fd=open(argv[1],O_RDONLY);
    long long size=lseek(fd,0,SEEK_END);
    printf("size=%lld\n",size);
    input=(char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
    //cout<<"p1"<<endl;//
    //划分数据
    long long instart[NUM_THREADS];
    instart[0]=0;
    adjStart[0]=0;
    for(long long i=1;i<NUM_THREADS;++i)
    {
        instart[i]=(size/NUM_THREADS)*i;
        //移至下一行首
        while(*(input+instart[i])!='\n')
        {instart[i]++;}
        instart[i]++;
        //获取这个数字
        string This="";
        while(*(input+instart[i])!=' ')
        {
            This+=*(input+instart[i]);
            instart[i]++;
        }
        //往下移，直到行首不是这个数字
        while(1)
        {
            string Next="";
            while(*(input+instart[i])!='\n')
            {instart[i]++;}
            instart[i]++;
            while(*(input+instart[i])!=' ')
            {
                Next+=*(input+instart[i]);
                instart[i]++;
            }
            if(Next!=This)
            {
                adjStart[i]=stoi(Next);
                while(*(input+instart[i])!='\n')
                {instart[i]--;}
                instart[i]++;
                break;
            }
        }
    }
    //cout<<"p1"<<endl;//
    long long inend[NUM_THREADS];
    for(int i=0;i<NUM_THREADS-1;++i)
    {inend[i]=instart[i+1];}
    inend[NUM_THREADS-1]=size;
    for(int i=0;i<NUM_THREADS-1;++i)
    {adjSize[i]=adjStart[i+1]-adjStart[i];}
    adjSize[NUM_THREADS-1]=n-adjStart[NUM_THREADS-1];
    //print threads some message
    for(int i=0;i<NUM_THREADS;++i)
    {
        printf("Thread %d:Start: %lld Size: %lld\n",i,adjStart[i],adjSize[i]);
    }
    //work
    #pragma omp parallel for
    for(int i=0;i<NUM_THREADS;++i)
    {
        //vector<node>adj[NUM_THREADS];
        long long shift=instart[i];
        long long shiftend=inend[i];
        while(1)
        {
            string start,end;
            while(1)
            {
                start+=*(input+shift);
                shift++;
                if(*(input+shift)==' '||*(input+shift)=='\t')//解决\t和空格的不一致性
                {shift++;break;}
            }
            while(*(input+shift)==' '||*(input+shift)=='\t')
            {shift++;}//两数间可能不止一个空格或\t,增强兼容性
            while(1)
            {
                end+=*(input+shift);
                shift++;
                if(*(input+shift)=='\n')
                {shift++;break;}
            }
            int s=stoi(start);
            int e=stoi(end);
            adjlist[s].list.push_back(e);
            adjlist[s].exist=true;
            //break
            if(shift>=shiftend)
            {break;}
        }
    }
    close(fd);
    munmap(input,size);
    printf("all data input successfully\n");
    //PriList();
    printf("%4d nodes\t%4d edges\n",n,m);

    double ott1=omp_get_wtime();
    DFS(atoi(argv[5]));
    double ott2=omp_get_wtime();
    cout<<"DFStime= "<<ott2-ott1<<endl<<"---over  DFS---"<<endl;
    printf("res has %d edges\n",resnums);

    // //out file
    // int outfd=open(argv[2],O_RDWR | O_CREAT | O_TRUNC,0777);
    // //划分数据
    // int outstart[NUM_THREADS];
    // int outend[NUM_THREADS];
    // outstart[0]=0;
    // for(int i=1;i<NUM_THREADS;++i)
    // {outstart[i]=i*res.size()/NUM_THREADS;}
    // outend[NUM_THREADS-1]=res.size()-1;
    // for(int i=0;i<NUM_THREADS-1;++i)
    // {outend[i]=outstart[i+1]-1;}
    // //work
    // string ompres[NUM_THREADS]={""};
    // omp_set_num_threads(NUM_THREADS);
    // #pragma omp parallel for
    // for(int i=0;i<NUM_THREADS;++i)
    // {
    //     for(int j=outstart[i];j<outend[i];++j)
    //     {ompres[i]+=to_string(res[j])+"\n";}
    //     ompres[i]+=to_string(res[outend[i]]);
    // }

    // for(int i=0;i<NUM_THREADS;++i)
    // {
    //     int outsize=lseek(outfd,0,SEEK_END);
    //     lseek(outfd,ompres[i].size(),SEEK_END);
    //     write(outfd,"\n",1);
    //     char *outfile=(char *)mmap(NULL,outsize+ompres[i].size(),PROT_READ | PROT_WRITE,MAP_SHARED,outfd,0);
    //     memcpy(outfile+outsize,ompres[i].c_str(),ompres[i].size());
    // }
    // close(outfd);

    //out file
    int outfd=open(argv[2],O_RDWR | O_CREAT | O_TRUNC,0777);
    lseek(outfd,res.size(),SEEK_END);
    //int outsize=lseek(outfd,0,SEEK_END);//0
    write(outfd,"\n",1);
    char *outfile=(char *)mmap(NULL,/*outsize+*/res.size(),PROT_READ | PROT_WRITE,MAP_SHARED,outfd,0);
    memcpy(outfile/*+outsize*/,res.c_str(),res.size());
    close(outfd);
    munmap(outfile,res.size());
    // //划分数据
    // int outstart[NUM_THREADS];
    // int outend[NUM_THREADS];
    // outstart[0]=0;
    // for(int i=1;i<NUM_THREADS;++i)
    // {
    //     outstart[i]=i*res.size()/NUM_THREADS;
    //     while(res[outstart[i]]!='\n')
    //     {outstart[i]++;}
    //     outstart[i]++;
    // }
    // outend[NUM_THREADS-1]=res.size()-2;
    // for(int i=0;i<NUM_THREADS-1;++i)
    // {outend[i]=outstart[i+1]-1;}
    //work
    //string ompres[NUM_THREADS]={""};
    // omp_set_num_threads(NUM_THREADS);
    // #pragma omp parallel for
    // for(int i=0;i<NUM_THREADS;++i)
    // {
    //     for(int j=outstart[i];j<outend[i];++j)
    //     {ompres[i]+=to_string(res[j])+"\n";}
    //     ompres[i]+=to_string(res[outend[i]]);
    // }
    // for(int i=0;i<NUM_THREADS;++i)
    // {
    //     int outsize=lseek(outfd,0,SEEK_END);
    //     lseek(outfd,ompres[i].size(),SEEK_END);
    //     write(outfd,"\n",1);
    //     char *outfile=(char *)mmap(NULL,outsize+ompres[i].size(),PROT_READ | PROT_WRITE,MAP_SHARED,outfd,0);
    //     memcpy(outfile+outsize,ompres[i].c_str(),ompres[i].size());
    // }
    // close(outfd);

    //isolate
    long long isolate=0;
    for(int i=0;i<n;++i)
    {
        if(!adjlist[i].flag&&adjlist[i].exist)
        {
            isolate++;
            //printf("warning:node %d is isolated\n",i);
        }
    }
    printf("%lld nodes isolated\n",isolate);
    //time
    double ot2=omp_get_wtime();
    printf("ALL TIME:%lf\n",ot2-ot1);

    return 0;
}