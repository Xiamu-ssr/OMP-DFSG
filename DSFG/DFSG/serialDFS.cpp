#include<cstdio>
#include<cstdlib>
#include<vector>
#include<stack>
#include<ctime>
#include<iostream>
#include<fstream>
#include<string>
#include<chrono>

using namespace std;
vector<int>res;
struct node
{
    vector<int>list;
    bool flag=false;//是否已访问
    bool exist=false;
};
vector<node>adjlist;//邻接表

void PriList()
{
    for(int i=0;i<adjlist.size();++i)
    {
        for(int j=0;j<adjlist[i].list.size();++j)
        {
            printf("(%2d,%2d)\n",i,adjlist[i].list[j]);
        }
    }
}
void DFS(int start)
{
    printf("---begin DFS---\n");
    adjlist[start].flag=true;
    //printf("%4d\t",start);
    res.push_back(start);
    stack<int>sck;
    sck.push(start);
    while(!sck.empty())
    {
        int now=sck.top();
        //printf("now is node %d\n",now);
        bool f=false;//如果从此点出发的边都已走过,则出栈
        for(int i=0;i<adjlist[now].list.size();++i)//size()如果每次都要遍历得到长度的话，可以考虑手动换掉
        {
            if(!adjlist[adjlist[now].list[i]].flag)
            {
                adjlist[adjlist[now].list[i]].flag=true;
                sck.push(adjlist[now].list[i]);
                //printf("%4d\t",adjlist[now].list[i]);
                res.push_back(adjlist[now].list[i]);
                f=1;
                break;
            }
        }
        if(!f)
        {sck.pop();}
    }
}

int main(int argc,char* argv[])
{
    time_t t1=clock();
    int n=atoi(argv[3]),m=atoi(argv[4]);
    for(int i=0;i<n;++i)
    {
        node a;
        adjlist.push_back(a);
    }
    string filename=argv[1];
    ifstream input_file(filename);
    if (!input_file.is_open())
    {cerr << "Could not open the file - '"<< filename << "'" << endl;return EXIT_FAILURE;}
    for(int i=0;i<m;++i)
    {
        int start,end;
        input_file>>start;
        input_file>>end;
        adjlist[start].list.push_back(end);
        adjlist[start].exist=true;
        // adjlist[end].list.push_back(start);
        // adjlist[end].exist=true;
    }
    input_file.close();
    printf("all data input successfully\n");
    //PriList();
    printf("%4d nodes\t%4d edges\n",n,m);
    time_t tt1=clock();
    // clock_t t1,t2;
    // t1=clock();
    DFS(atoi(argv[5]));
    time_t tt2=clock();
    cout<<"DFStime= "<<(double)(tt2-tt1)/CLOCKS_PER_SEC<<endl<<"---over  DFS---"<<endl;
    // clock_t t1,t2;
    // t1=clock();
    // DFS(0);
    // t2=clock();
    // printf("time=%lf\n---over  DFS---\n",(double)(t2-t1)/CLOCKS_PER_SEC);
    printf("res has %ld edges\n",res.size());
    ofstream out_file(argv[2]);
    for(int i=0;i<res.size();++i)
    {
        out_file<<res[i]<<endl;
    }
    int isolate_num=0;
    for(int i=0;i<n;++i)
    {
        if(!adjlist[i].flag&&adjlist[i].exist)
        {
            //printf("warning:node %d is isolated\n",i);
            isolate_num++;
        }
    }
    cout<<"there are "<<isolate_num<<" nodes are isolated!"<<endl;
    out_file.close();
    //time
    time_t t2=clock();
    printf("ALLTIME:%lf\n",(double)(t2-t1)/CLOCKS_PER_SEC);
    return 0;
}