#include<cstdio>
#include<cstdlib>
#include<vector>
#include<ctime>
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<omp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
using namespace std;

#define NUM_THREADS 4
static omp_lock_t lock;
map<string,string>mp;

int main(int argc,char* argv[])
{
    //if(argc>5){printf("error:input nums must equal 4!\n");return 0;}
    double ot1=omp_get_wtime();
    time_t t1=clock();
    /*读取映射表*/
    //mmap init
    char *dict=NULL;
    int fd=open("dict.txt",O_RDONLY);
    long long size=lseek(fd,0,SEEK_END);
    printf("size=%lld\n",size);
    dict=(char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
    //划分数据
    long long instart[NUM_THREADS];
    instart[0]=0;
    for(long long i=1;i<NUM_THREADS;++i)
    {
        instart[i]=(size*i)/NUM_THREADS;
        while(*(dict+instart[i])!='\n')
        {instart[i]++;}
        instart[i]++;
    }
    long long inend[NUM_THREADS];
    for(int i=0;i<NUM_THREADS-1;++i)
    {inend[i]=instart[i+1];}
    inend[NUM_THREADS-1]=size;
    //work
    #pragma omp parallel for
    for(int i=0;i<NUM_THREADS;++i)
    {
        map<string,string>mpsmall;
        long long shift=instart[i];
        long long shiftend=inend[i];
        printf("thread=%d\n",i);
        while(1)
        {
            string start,end;
            while(1)
            {
                start+=*(dict+shift);
                shift++;
                if(*(dict+shift)==' '||*(dict+shift)=='\t')//解决\t和空格的不一致性
                {shift++;break;}
            }
            while(*(dict+shift)==' '||*(dict+shift)=='\t')
            {shift++;}//两数间可能不止一个空格或\t,增强兼容性
            while(1)
            {
                end+=*(dict+shift);
                shift++;
                if(*(dict+shift)=='\n')
                {shift++;break;}
            }
            mpsmall[start]=end;
            if(shift>=shiftend)
            {break;}
        }
        omp_set_lock(&lock);
        mp.insert(mpsmall.begin(),mpsmall.end());
        omp_unset_lock(&lock);
    }
    close(fd);
    munmap(dict,size);

    /*使用map映射数据*/
    //mmap init
    char *input=NULL;
    int fdin=open(argv[1],O_RDONLY);
    size=lseek(fdin,0,SEEK_END);
    printf("size=%lld\n",size);
    input=(char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fdin,0);
    //划分数据
    instart[0]=0;
    for(long long i=1;i<NUM_THREADS;++i)
    {
        instart[i]=(size*i)/NUM_THREADS;
        while(*(input+instart[i])!='\n')
        {instart[i]++;}
        instart[i]++;
    }
    for(int i=0;i<NUM_THREADS-1;++i)
    {inend[i]=instart[i+1];}
    inend[NUM_THREADS-1]=size;
    //work
    ofstream out_file(argv[2]);
    if (!out_file.is_open())
    {cerr << "Could not open the file - '"<< argv[2] << "'" << endl;return EXIT_FAILURE;}
    string res[NUM_THREADS];
    #pragma omp parallel for
    for(int i=0;i<NUM_THREADS;++i)
    {
        res[i]="";
        long long shift=instart[i];
        long long shiftend=inend[i];
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
            res[i]+=mp[start]+" "+mp[end]+"\n";
            if(shift>=shiftend)
            {break;}
        }
    }
    for(int i=0;i<NUM_THREADS;++i)
    {out_file<<res[i];}
    close(fdin);
    munmap(input,size);
    out_file.close();
    //time
    double ot2=omp_get_wtime();
    time_t t2=clock();
    printf("TIME:%lf\n---over write---\n",(double)(t2-t1)/CLOCKS_PER_SEC);
    printf("OMP TIME:%lf\n---over write---\n",ot2-ot1);
    return 0;
}