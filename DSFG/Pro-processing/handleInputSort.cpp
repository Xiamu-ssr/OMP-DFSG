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
map<string,int>mp;
struct node
{
    vector<int>list;
    bool flag=false;//是否已访问
    bool exist=false;
};
vector<node>adjlist;//邻接表
int nodes=0,edges=0;

int main(int argc,char* argv[])
{
    nodes=stoi(argv[3]);
    edges=stoi(argv[4]);
    //if(argc>5){printf("error:input nums must equal 4!\n");return 0;}
    double ot1=omp_get_wtime();
    time_t t1=clock();
    /*读取映射表*/
    ifstream in_file2("dict.txt");
    for(int i=0;i<nodes;++i)
    {
        string start;
        int end;
        in_file2>>start>>end;
        mp[start]=end;
    }
    in_file2.close();
    printf("dict ok\n");
    // //mmap init
    // char *dict=NULL;
    // int fd=open("dict.txt",O_RDONLY);
    // long long size=lseek(fd,0,SEEK_END);
    // printf("size=%lld\n",size);
    // dict=(char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
    // //划分数据
    // long long instart[NUM_THREADS];
    // instart[0]=0;
    // for(long long i=1;i<NUM_THREADS;++i)
    // {
    //     instart[i]=(size*i)/NUM_THREADS;
    //     while(*(dict+instart[i])!='\n')
    //     {instart[i]++;}
    //     instart[i]++;
    // }
    // long long inend[NUM_THREADS];
    // for(int i=0;i<NUM_THREADS-1;++i)
    // {inend[i]=instart[i+1];}
    // inend[NUM_THREADS-1]=size;
    // //work
    // #pragma omp parallel for
    // for(int i=0;i<NUM_THREADS;++i)
    // {
    //     map<string,string>mpsmall;
    //     long long shift=instart[i];
    //     long long shiftend=inend[i];
    //     printf("thread=%d\n",i);
    //     while(1)
    //     {
    //         string start,end;
    //         while(1)
    //         {
    //             start+=*(dict+shift);
    //             shift++;
    //             if(*(dict+shift)==' '||*(dict+shift)=='\t')//解决\t和空格的不一致性
    //             {shift++;break;}
    //         }
    //         while(*(dict+shift)==' '||*(dict+shift)=='\t')
    //         {shift++;}//两数间可能不止一个空格或\t,增强兼容性
    //         while(1)
    //         {
    //             end+=*(dict+shift);
    //             shift++;
    //             if(*(dict+shift)=='\n')
    //             {shift++;break;}
    //         }
    //         mpsmall[start]=end;
    //         if(shift>=shiftend)
    //         {break;}
    //     }
    //     omp_set_lock(&lock);
    //     mp.insert(mpsmall.begin(),mpsmall.end());
    //     omp_unset_lock(&lock);
    // }
    // close(fd);
    // munmap(dict,size);

    /*使用map映射数据*/
    for(int i=0;i<nodes;++i)
    {node a;adjlist.push_back(a);}
    ifstream in_file(argv[1]);
    if (!in_file.is_open())
    {cerr << "Could not open the file - '"<< argv[1] << "'" << endl;return EXIT_FAILURE;}
    ofstream out_file(argv[2]);
    if (!out_file.is_open())
    {cerr << "Could not open the file - '"<< argv[2] << "'" << endl;return EXIT_FAILURE;}
    for(int i=0;i<edges;++i)
    {
        string start,end;
        in_file>>start>>end;
        adjlist[mp[start]].list.push_back(mp[end]);
        //adjlist[mp[end]].list.push_back(mp[start]);
    }
    printf("in ok\n");
    for(int i=0;i<nodes;++i)
    {
        for(int j=0;j<adjlist[i].list.size();++j)
        {out_file<<i<<" "<<adjlist[i].list[j]<<endl;}
    }
    
    in_file.close();
    out_file.close();
    //time
    double ot2=omp_get_wtime();
    time_t t2=clock();
    printf("TIME:%lf\n---over write---\n",(double)(t2-t1)/CLOCKS_PER_SEC);
    printf("OMP TIME:%lf\n---over write---\n",ot2-ot1);
    return 0;
}