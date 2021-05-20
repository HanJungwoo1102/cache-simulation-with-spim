#include "spim.h"
#include "pipeline.h"


void print_result(int n_cycle, int n_dstall, int n_bstall)
{
	printf("Number of Cycle : %d\n", n_cycle);	
	printf("Number of Stall by Data Hazard : %d\n", n_dstall);
	printf("Number of Stall by Branch (Jump) : %d\n", n_bstall);
}
