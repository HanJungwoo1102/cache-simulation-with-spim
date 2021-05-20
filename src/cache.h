

#ifndef __cache__
#define __cache__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exported functions for cache */
int data_load(unsigned int);			// data load operation
int data_store(unsigned int);			// data store operation
int instruction_load(unsigned int);	// instruction load operation
void print_cache_result(int n_cycles);		// print final result of hit/miss ratio

#endif
