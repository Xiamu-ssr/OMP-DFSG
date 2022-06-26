#include <omp.h>
#include <cstdio>
#define COUNT 1
 
int main()  
{

#pragma omp parallel for schedule(static,4)
	for(int i = 0;i < COUNT; i++) 
	{
		printf("Thread: %d, Iteration: %d\n", omp_get_thread_num(), i);
	}
 
	return 0;  
}