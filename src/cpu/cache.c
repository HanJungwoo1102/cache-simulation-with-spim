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
	bool dirty;
	int tag;
	Data* data;
	int lengthOfData;
} Block;

typedef struct CacheConfig {
	int size;
	int numberOfEntries;
	int numberOfWay;
	ReplacementPolicy replacementPolicy;
	WritePolicy writePolicy;
	int cacheHitTime;
} CacheConfig;

typedef struct WaySet {
	Block* blocks;
	int* recentlyUsed;
	int firstInIndex;
} WaySet;

typedef struct Result {
	int accessCount;
	int hitCount;
} Result;

typedef struct Cache {
	CacheConfig config;
	WaySet* entries;
	int tagSize;
	int indexSize;
	int blockOffsetSize;
	Result result;
	Cache* nextLevelCache;
} Cache;

typedef struct CacheSystem {
	Cache* L1InstructionCache;
	Cache* L1DataCache;
	int numberOfLevels;
	int memoryAccessTime;
} CacheSystem;

bool isCacheSystemCreated = false;
int instructionCount = 0;
CacheSystem cacheSystem;

int getLog(int src) {
	int i;
	int count = 0;

	while(src != 1) {
		src = src >> 1;
		count += 1;
	}

	return count;
}

void printCacheConfig(CacheConfig config) {
	char* replacementPolicy;
	char* writePolicy;
	if (config.replacementPolicy == LRU) {
		replacementPolicy = "LRU";
	} else if (config.replacementPolicy == FIFO) {
		replacementPolicy = "FIFO";
	}
	if (config.writePolicy == WT) {
		writePolicy = "WT";
	} else if (config.writePolicy == WB) {
		writePolicy = "WB";
	}
	printf("%d %d %d %s %s %d\n", config.size, config.numberOfEntries, config.numberOfWay, replacementPolicy, writePolicy, config.cacheHitTime);
}

Cache* createCache(CacheConfig cacheConfig) {
	Cache* cache = (Cache *)malloc(sizeof(Cache));
	cache->config = cacheConfig;

	int lengthOfData = cacheConfig.size / cacheConfig.numberOfEntries / cacheConfig.numberOfWay / sizeof(Data);

	WaySet* entries = (WaySet *) malloc(sizeof(WaySet) * cacheConfig.numberOfEntries);

	int i, j, k;
	for (i = 0; i < cacheConfig.numberOfEntries; i++) {
		Block* blocks = (Block *) malloc(sizeof(Block) * cacheConfig.numberOfWay);
		int* recentlyUsed = (int *) malloc(sizeof(int) * cacheConfig.numberOfWay);

		for (j = 0; j < cacheConfig.numberOfWay; j++) {
			Data* data = (Data *) malloc(sizeof(Data) * lengthOfData);
		
			for (k = 0; k < lengthOfData; k++) {
				data[k] = 0;
			}

			blocks[j].tag = 0;
			blocks[j].data = data;
			blocks[j].lengthOfData = lengthOfData;
			blocks[j].valid = false;
			blocks[j].dirty = false;
		}

		for (j = 0; j < cacheConfig.numberOfWay; j++) {
			recentlyUsed[j] = 0;
		}

		entries[i].blocks = blocks;
		entries[i].recentlyUsed = recentlyUsed;
		entries[i].firstInIndex = 0;
	}

	cache->blockOffsetSize = getLog(lengthOfData);
	cache->indexSize = getLog(cacheConfig.numberOfEntries);
	cache->result.accessCount = 0;
	cache->result.hitCount = 0;
	cache->entries = entries;
	cache->nextLevelCache = NULL;

	return cache;
}

void loadCacheConfig(int* numberOfLevels, int* memoryAccessTime, CacheConfig* l1CacheConfig, CacheConfig* l2CacheConfig) {
	int i;
	char buffer[100];
	FILE* file = fopen("../CPU/cache.config", "r");

	if(file == NULL){
    printf("파일열기 실패\n");
		return;
  }

	fgets(buffer, sizeof(buffer), file);
	char* temp = strtok(buffer, " ");
	*numberOfLevels = atoi(temp);
	temp = strtok(NULL, " ");
	*memoryAccessTime = atoi(temp);

	for (i = 0; i < *numberOfLevels; i++) {
		CacheConfig* c;
		if (i == 0) {
			c = l1CacheConfig;
		} else {
			c = l2CacheConfig;
		}
		fgets(buffer, sizeof(buffer), file);
		temp = strtok(buffer, " ");
		c->size = atoi(temp);
		temp = strtok(NULL, " ");
		c->numberOfEntries = atoi(temp);
		temp = strtok(NULL, " ");
		c->numberOfWay = atoi(temp);
		temp = strtok(NULL, " ");
		if (strcmp(temp, "FIFO") == 0) {
			c->replacementPolicy = FIFO;
		} else if (strcmp(temp, "LRU") == 0) {
			c->replacementPolicy = LRU;
		}
		temp = strtok(NULL, " ");
		if (strcmp(temp, "WT") == 0) {
			c->writePolicy = WT;
		} else if (strcmp(temp, "WB") == 0) {
			c->writePolicy = WB;
		}
		temp = strtok(NULL, " ");
		if (i == 0) {
			c->cacheHitTime = 0;
		} else {
			c->cacheHitTime = atoi(temp);
		}
	}

}

void createCacheSystem() {
	CacheConfig l1CacheConfig, l2CacheConfig;
	int numberOfLevels, memoryAccessTime;

	loadCacheConfig(&numberOfLevels, &memoryAccessTime, &l1CacheConfig, &l2CacheConfig);

	printCacheConfig(l1CacheConfig);
	printCacheConfig(l2CacheConfig);

	Cache* L1DataCache = NULL;
	Cache* L1InstructionCache = NULL;
	Cache* L2Cache = NULL;

	cacheSystem.L1DataCache = createCache(l1CacheConfig);
	cacheSystem.L1InstructionCache = createCache(l1CacheConfig);

	if (numberOfLevels == 2) {
		L2Cache = createCache(l2CacheConfig);
	}
	cacheSystem.numberOfLevels = numberOfLevels;
	cacheSystem.memoryAccessTime = memoryAccessTime;
	cacheSystem.L1DataCache->nextLevelCache = L2Cache;
	cacheSystem.L1InstructionCache->nextLevelCache = L2Cache;

	isCacheSystemCreated = true;
}

void printCache(Cache* cache) {
	int i, j, k;
	for (i = 0; i < cache->config.numberOfEntries; i++) {
		WaySet waySet = cache->entries[i];
		printf(" [%d] ||", i);
		for (j = 0; j < cache->config.numberOfWay; j++) {
			Block block = waySet.blocks[j];
			printf(" (%d) (%d) (0x%x) ", block.valid, block.dirty, block.tag);
			for (k = 0; k < block.lengthOfData; k++) {
				printf(" %d ", block.data[k]);
			}
			printf("|");
		}
		printf("\n");
	}
}

void printCacheSystem() {
	printf("\nLevel 1 Instruction Cache\n");
	printCache(cacheSystem.L1InstructionCache);
	
	printf("Level 1 Data Cache\n");
	printCache(cacheSystem.L1DataCache);

	if (cacheSystem.numberOfLevels == 2) {
		printf("Level 2 Data Cache\n");
		printCache(cacheSystem.L1DataCache->nextLevelCache);
	} 

	printf("-------------------------------\n");
}

int getTag(Cache cache, unsigned int addr) {
	return addr >> (2 + cache.blockOffsetSize + cache.indexSize);
}

int getIndex(Cache cache, unsigned int addr) {
	return (addr >> (2 + cache.blockOffsetSize)) & (cache.config.numberOfEntries - 1);
}

int change(Cache* cache, unsigned int addr) {
	int i;
	int stallCycles = cache->config.cacheHitTime;
	int index = getIndex(*cache, addr);
	int tag = getTag(*cache, addr);

	if (cache->config.writePolicy == WT) {
		if (cache->nextLevelCache == NULL) {
			stallCycles += cacheSystem.memoryAccessTime;
		} else {
			stallCycles += change(cache->nextLevelCache, addr);
		}
		printf(" (change WT) ");
	} else if (cache->config.writePolicy == WB) {
		for (i = 0; i < cache->config.numberOfWay; i++) {
			if (cache->entries[index].blocks[i].tag == tag) {
				cache->entries[index].blocks[i].dirty = true;
				printf(" (change WB) " );
			}
		}
	}

	return stallCycles;
}

int insert(Cache* cache, int index, int tag, unsigned int addr) {
	int blockPlace, i;
	int stallCycles = 0;

	WaySet waySet = cache->entries[index];

	for (blockPlace = 0; blockPlace < cache->config.numberOfWay; blockPlace++) {
		if (waySet.blocks[blockPlace].valid == false) break;
	}

	if (blockPlace == cache->config.numberOfWay) {
		if (cache->config.replacementPolicy == LRU) {
			int leastUsed = waySet.recentlyUsed[0];
			blockPlace = 0;
			for (i = 0; i < cache->config.numberOfWay; i++) {
				if (waySet.recentlyUsed[i] < leastUsed) {
					blockPlace = i;
					leastUsed = waySet.recentlyUsed[i];
				}
			}
			printf(" (replace LRU %dth block) ", blockPlace);
		} else if (cache->config.replacementPolicy == FIFO) {
			blockPlace = waySet.firstInIndex;
			cache->entries[index].firstInIndex = (waySet.firstInIndex == cache->config.numberOfWay)
				? 0
				: waySet.firstInIndex + 1;
			printf(" (replace FIFO %dth block) ", blockPlace);
		}
	}

	if (waySet.blocks[blockPlace].dirty) {
		if (cache->nextLevelCache == NULL) {
			stallCycles += cacheSystem.memoryAccessTime;
		} else {
			stallCycles += change(cache->nextLevelCache, addr);
		}
	}

	cache->entries[index].blocks[blockPlace].tag = tag;
	cache->entries[index].blocks[blockPlace].valid = true;
	cache->entries[index].blocks[blockPlace].dirty = false;
	printf(" (insert %x, %x, %d, %d) ", index, tag, blockPlace, instructionCount);
	return stallCycles;
}

bool access(Cache* cache, int index, int tag) {
	int i;
	int stallCycle = cache->config.cacheHitTime;

	cache->result.accessCount += 1;
	for (i = 0; i < cache->config.numberOfWay; i++) {
		Block block = cache->entries[index].blocks[i];

		if (block.tag == tag) {
			cache->result.hitCount += 1;
			cache->entries[index].recentlyUsed[i] = cache->result.accessCount;
			printf(" (hit!!) ");
			return true;
		}
	}
	printf(" (missㅠㅠ) ");
	return false;
}

int loadCache(Cache* cache, unsigned int addr) {
	int stallCycle = cache->config.cacheHitTime;
	int index = getIndex(*cache, addr);
	int tag = getTag(*cache, addr);

	if (!access(cache, index, tag)) {
		if (cache->nextLevelCache == NULL) {
			stallCycle += cacheSystem.memoryAccessTime;
		} else {
			stallCycle += loadCache(cache->nextLevelCache, addr);
		}
		stallCycle += insert(cache, index, tag, addr);
	}

	return stallCycle;
}

int loadInstCache(unsigned int addr) {
	return loadCache(cacheSystem.L1InstructionCache, addr);
}

int loadDataCache(unsigned int addr) {
	return loadCache(cacheSystem.L1DataCache, addr);
}

int storeDataCache(unsigned int addr) {
	return change(cacheSystem.L1DataCache, addr);
}

int data_load (unsigned int addr) {
	/* You have to implement your own data_load function here! */
	int stallCycles = 0;
	instructionCount += 1;
	if (instructionCount < 8) return 0;

	if (!isCacheSystemCreated) {
		createCacheSystem();
	}

	stallCycles += loadDataCache(addr);

	printf("\nLOAD DATA - 0x%x (%d)\n", addr, stallCycles);
	printCacheSystem();

	return stallCycles;
	// Return value: stall cycles due to L1 cache miss
	///////////////////////////////////////////////////////////
}

int instruction_load (unsigned int addr) {
	/* You have to implement your own instruction_load function here! */
	int stallCycles = 0;
	instructionCount += 1;
	if (instructionCount < 8) return 0;
	
	if (!isCacheSystemCreated) {
		createCacheSystem();
	}

	stallCycles += loadInstCache(addr);

	printf("\nLOAD INST - 0x%x (%d)\n", addr, stallCycles);
	printCacheSystem();

	return stallCycles;
	// Return value: stall cycles due to L1 cache miss 
	///////////////////////////////////////////////////////////
}

int data_store(unsigned int addr) {
	/* You have to implement your own data_store function here! */
	int stallCycles = 0;
	instructionCount += 1;
	if (instructionCount < 8) return 0;

	if (!isCacheSystemCreated) {
		createCacheSystem();
	}

	stallCycles += loadDataCache(addr);
	stallCycles += storeDataCache(addr);
	
	printf("\nSTORE DATA - 0x%x (%d)\n", addr, stallCycles);
	printCacheSystem();

	return stallCycles;
	// Return value: stall cycles due to L1 cache miss
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
	float l2HitRate;
	instructionCount += 1;

	if (!isCacheSystemCreated) {
		createCacheSystem();
	}
	float l1iAccessCount = cacheSystem.L1InstructionCache->result.accessCount;
	float l1iHitCount = cacheSystem.L1InstructionCache->result.hitCount;
	float l1iHitRate = l1iHitCount / l1iAccessCount;

	float l1dAccessCount = cacheSystem.L1DataCache->result.accessCount;
	float l1dHitCount = cacheSystem.L1DataCache->result.hitCount;
	float l1dHitRate = l1dHitCount / l1dAccessCount;

	if (cacheSystem.numberOfLevels == 2) {
		float l2AccessCount = cacheSystem.L1DataCache->nextLevelCache->result.accessCount;
		float l2HitCount = cacheSystem.L1DataCache->nextLevelCache->result.hitCount;
		l2HitRate = l2HitCount / l2AccessCount;
	}

	float totalAccessCount = cacheSystem.L1InstructionCache->result.accessCount + cacheSystem.L1DataCache->result.accessCount;
	float totalHitCount = cacheSystem.L1InstructionCache->result.hitCount + cacheSystem.L1DataCache->result.hitCount;
	if (cacheSystem.numberOfLevels == 2) {
		totalHitCount += cacheSystem.L1DataCache->nextLevelCache->result.hitCount;
	}
	float totalHitRate = totalHitCount / totalAccessCount;

	printf("\n\n");
	printf("Level 1 Cache\n");
	printf("Hit Count of l1i-cache: %d\n", cacheSystem.L1InstructionCache->result.hitCount);
	printf("Miss Count of l1i-cache: %d\n", cacheSystem.L1InstructionCache->result.accessCount - cacheSystem.L1InstructionCache->result.hitCount);
	printf("Hit Ratio of l1i-cache: %0.3f\n", l1iHitRate);
	printf("\n");
	printf("Hit Count of l1d-cache: %d\n", cacheSystem.L1DataCache->result.hitCount);
	printf("Miss Count of l1d-cache: %d\n", cacheSystem.L1DataCache->result.accessCount - cacheSystem.L1DataCache->result.hitCount);
	printf("Hit Ratio of l1d-cache: %0.3f\n", l1dHitRate);
	printf("\n");
	printf("Level 2 Cache\n");
	if (cacheSystem.numberOfLevels == 2) {
		printf("Hit Count: %d\n", cacheSystem.L1DataCache->nextLevelCache->result.hitCount);
		printf("Miss Count: %d\n", cacheSystem.L1DataCache->nextLevelCache->result.accessCount - cacheSystem.L1DataCache->nextLevelCache->result.hitCount);
		printf("Hit Ratio: %0.3f\n", l2HitRate);
	}
	printf("\n");
	printf("Total Hit Ratio: %0.3f\n", totalHitRate);

	//////////////////////////////////////////////////////////////////////
}
