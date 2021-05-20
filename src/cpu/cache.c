#include "cache.h"
#include "spim.h"
#include "run.h"

int data_load (unsigned int addr) {
	/* You have to implement your own data_load function here! */
	
	return 0;		// Return value: stall cycles due to L1 cache miss
	///////////////////////////////////////////////////////////
}

int instruction_load (unsigned int addr) {
	/* You have to implement your own instruction_load function here! */

	return 0;		// Return value: stall cycles due to L1 cache miss 
	///////////////////////////////////////////////////////////
}

int data_store(unsigned int addr) {
	/* You have to implement your own data_store function here! */

	return 0;		// Return value: stall cycles due to L1 cache miss
	///////////////////////////////////////////////////////////
}

void print_cache_result(int n_cycles) {
	/* You have to print the result of hit/miss count of each cache. You have to follow the format as below example.
	Calculate hit ratio down to three places of decimals.
	Example)
	Level 1 Cache
	Hit Count of l1i-cache: 4
	Miss Count of l1i-cache: 5
	Hit Ratio of l1i-cache: 0.444

	Hit Count of l1d-cache: 1
	Miss Count of l1d-cache: 3
	Hit Ratio of l1d-cache: 0.250

	Level 2 Cache
	Hit Count: 0
	Miss Count: 8
	Hit Ratio: 0.000

	Total Hit Ratio: 0.385
	*/

	/* You have to implement your own print_cache_result function here! */



	//////////////////////////////////////////////////////////////////////
}
