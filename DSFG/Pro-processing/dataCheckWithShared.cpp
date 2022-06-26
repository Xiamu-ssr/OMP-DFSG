#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <iostream>
#include <omp.h>
#include <map>
#include <string>
#include <ctime>
using namespace std;
#define NUM_THREADS 4
map<string,bool>mp;
long long edges=0,nodes=0;
static omp_lock_t lock;

int main(int argc, char** args)
{
    double ot1=omp_get_wtime();
    time_t t1=clock();
    //mmap init
    char *input=NULL;
    int fd=open(args[1],O_RDONLY);
    long long size=lseek(fd,0,SEEK_END);
    printf("size=%lld\n",size);
    input=(char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);

    //划分数据
    long long instart[NUM_THREADS];
    instart[0]=0;
    for(long long i=1;i<NUM_THREADS;++i)
    {
        instart[i]=(size*i)/NUM_THREADS;
        while(*(input+instart[i])!='\n')
        {instart[i]++;}
        instart[i]++;
    }
    long long inend[NUM_THREADS];
    for(int i=0;i<NUM_THREADS-1;++i)
    {inend[i]=instart[i+1];}
    inend[NUM_THREADS-1]=size;

    //work
    omp_init_lock(&lock);
    omp_set_num_threads(NUM_THREADS);
    string Maxnode="0",Minnode="aaaaaaaaaaaaaaaaaaaaaa";
    #pragma omp parallel for
    for(int i=0;i<NUM_THREADS;++i)
    {
        string maxnode="0",minnode="aaaaaaaaaaaaaaaaaaaaaa";
        map<string,bool>mpsmall;
        long long shift=instart[i];
        long long shiftend=inend[i];
        int nums=0;
        printf("thread=%d\n",i);
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
            if(maxnode.length()<start.length())
            {maxnode=start;}
            else if(maxnode.length()==start.length())
            {maxnode=maxnode>start?maxnode:start;}
            if(maxnode.length()<end.length())
            {maxnode=end;}
            else if(maxnode.length()==end.length())
            {maxnode=maxnode>end?maxnode:end;}
            if(minnode.length()>start.length())
            {minnode=start;}
            else if(minnode.length()==start.length())
            {minnode=minnode<start?minnode:start;}
            if(minnode.length()>end.length())
            {minnode=end;}
            else if(minnode.length()==end.length())
            {minnode=minnode<end?minnode:end;}
            //omp_set_lock(&lock);
            mpsmall[start]=true;
            mpsmall[end]=true;
            //omp_unset_lock(&lock);
            nums++;
            if(shift>=shiftend)
            {break;}
        }
        omp_set_lock(&lock);
        if(Maxnode.length()<maxnode.length())
        {Maxnode=maxnode;}
        else if(Maxnode.length()==maxnode.length())
        {Maxnode=Maxnode>maxnode?Maxnode:maxnode;}
        if(Minnode.length()>minnode.length())
        {Minnode=minnode;}
        else if(Minnode.length()==minnode.length())
        {Minnode=Minnode<minnode?Minnode:minnode;}
        mp.insert(mpsmall.begin(),mpsmall.end());
        omp_unset_lock(&lock);
        edges+=nums;
        cout<<"nums="<<nums<<endl;
    }
    double ot2=omp_get_wtime();
    time_t t2=clock();
    //close everything
    close(fd);
    munmap(input,size);
    
    nodes=mp.size();
    printf("input file has %lld nodes and %lld edges\n",nodes,edges);
    cout<<"minnode is "<<Minnode<<" and maxnode is "<<Maxnode<<endl;
    cout<<"time="<<(double)(t2-t1)/CLOCKS_PER_SEC<<" s"<<endl;
    cout<<"omp time"<<ot2-ot1<<" s"<<endl;
    return 0;
}

