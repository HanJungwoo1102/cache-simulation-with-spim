#include "cache.h"
#include "spim.h"
#include "run.h"

typedef enum ReplacementPolicy {
  LRU, FIFO,
} ReplacementPolicy;

typedef enum WritePolicy {
	WT, WB,
} WritePolicy;

typedef int Data;

typedef struct Block {
	bool valid;
	bool dirt;
	int tag;
	Data* data;
	int lengthOfData;
} Block;

typedef struct CacheConfig {
	int size;
	int numberOfEntries;
	int numberOfWays;
	ReplacementPolicy replacementPolicy;
	WritePolicy writePolicy;
	int cacheHitTime;
} CacheConfig;

typedef struct Cache {
	CacheConfig config;
	Block** entries;
} Cache;

typedef struct CacheSystem {
	Cache L1InstructionCache;
	Cache L1DataCache;
	Cache L2Cache;
	int numberOfLevels;
	int memoryAccessTime;
} CacheSystem;

bool isCacheSystemCreated = false;
CacheSystem cacheSystem;

void createCache(Cache* cache, CacheConfig cacheConfig) {
	cache->config = cacheConfig;

	int lengthOfData = cacheConfig.size / cacheConfig.numberOfEntries / cacheConfig.numberOfWays / sizeof(Data);

	Block** entries = (Block **) malloc(sizeof(Block *) * cacheConfig.numberOfEntries);

	int i, j, k;
	for (int i = 0; i < cacheConfig.numberOfEntries; i++) {
		Block* blocks = (Block *) malloc(sizeof(Block) * cacheConfig.numberOfWays);
		
		for (j = 0; j < cacheConfig.numberOfWays; j++) {
			Data* data = (Data *) malloc(sizeof(Data) * lengthOfData);
		
			for (k = 0; k < lengthOfData; k++) {
				data[k] = 0;
			}

			blocks[j].tag = 0;
			blocks[j].data = data;
			blocks[j].lengthOfData = lengthOfData;
		}

		entries[i] = blocks;
	}

	cache->entries = entries;
}

void createCacheSystem() {
	int numberOfLevels = 2;
	int memoryAccessTime = 400;
	CacheConfig l1CacheConfig = { 16, 2, 1, LRU, WT, 1 };
	CacheConfig l2CacheConfig = { 64, 4, 2, FIFO, WB, 10 };

	cacheSystem.numberOfLevels = numberOfLevels;
	cacheSystem.memoryAccessTime = memoryAccessTime;

	createCache(&(cacheSystem.L1DataCache), l1CacheConfig);
	createCache(&(cacheSystem.L1InstructionCache), l1CacheConfig);

	if (numberOfLevels == 2) {
		createCache(&(cacheSystem.L2Cache), l2CacheConfig);
	}

	isCacheSystemCreated = true;
}

void printCacheSystem() {
	int i, j, k;
	printf("Level 1 Instruction Cache\n");
	for (i = 0; i < cacheSystem.L1InstructionCache.config.numberOfEntries; i++) {
		Block* entry = cacheSystem.L1InstructionCache.entries[i];
		for (j = 0; j < cacheSystem.L1InstructionCache.config.numberOfWays; j++) {
			Block block = entry[j];
			printf(" [0x%x] ", block.tag);
			for (k = 0; k < block.lengthOfData; k++) {
				printf(" %d ", block.data[k]);
			}
			printf("|");
		}
		printf("\n");
	}
	
	printf("Level 1 Data Cache\n");
	for (i = 0; i < cacheSystem.L1DataCache.config.numberOfEntries; i++) {
		Block* entry = cacheSystem.L1DataCache.entries[i];
		for (j = 0; j < cacheSystem.L1DataCache.config.numberOfWays; j++) {
			Block block = entry[j];
			printf(" [0x%x] ", block.tag);
			for (k = 0; k < block.lengthOfData; k++) {
				printf(" %d ", block.data[k]);
			}
			printf("|");
		}
		printf("\n");
	}

	printf("Level 2 Data Cache\n");
	for (i = 0; i < cacheSystem.L2Cache.config.numberOfEntries; i++) {
		Block* entry = cacheSystem.L2Cache.entries[i];
		for (j = 0; j < cacheSystem.L2Cache.config.numberOfWays; j++) {
			Block block = entry[j];
			printf(" [0x%x] ", block.tag);
			for (k = 0; k < block.lengthOfData; k++) {
				printf(" %d ", block.data[k]);
			}
			printf("|");
		}
		printf("\n");
	}
	printf("-------------------------------\n");
}

int data_load (unsigned int addr) {
	/* You have to implement your own data_load function here! */
	if (!isCacheSystemCreated) {
		createCacheSystem();
	}
	printf("LOAD DATA - 0x%x\n", addr);
	printCacheSystem();


	return 0;		// Return value: stall cycles due to L1 cache miss
	///////////////////////////////////////////////////////////
}

int instruction_load (unsigned int addr) {
	/* You have to implement your own instruction_load function here! */
	if (!isCacheSystemCreated) {
		createCacheSystem();
	}
	printf("LOAD INST - 0x%x\n", addr);
	printCacheSystem();

	return 0;		// Return value: stall cycles due to L1 cache miss 
	///////////////////////////////////////////////////////////
}

int data_store(unsigned int addr) {
	/* You have to implement your own data_store function here! */
	if (!isCacheSystemCreated) {
		createCacheSystem();
	}
	printf("STORE DATA - 0x%x\n", addr);
	printCacheSystem();

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
	if (!isCacheSystemCreated) {
		createCacheSystem();
	}


	//////////////////////////////////////////////////////////////////////
}
