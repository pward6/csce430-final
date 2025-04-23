#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

#define BLOCK_SIZE 16
#define CACHE_SIZE 4096
#define ADDRESS_BITS 32

#define NUM_BLOCKS (CACHE_SIZE / BLOCK_SIZE) // 256 blocks for direct-mapped
#define OFFSET_BITS 4                        // log2(16)
#define INDEX_BITS 8                         // log2(256)
#define TAG_BITS (ADDRESS_BITS - OFFSET_BITS - INDEX_BITS)

#define VICTIM_CACHE_SIZE 8

typedef struct {
    uint32_t tag;
    int valid;
} CacheBlock;

typedef struct {
    CacheBlock blocks[NUM_BLOCKS]; // direct mapped cache
} MainCache;

typedef struct {
    CacheBlock blocks[VICTIM_CACHE_SIZE];
    int next_to_replace;                 // helps with FIFO replacement
} VictimCache;

typedef struct {
    MainCache main;
    VictimCache victim;
} Cache;

// Functions
Cache* create_cache();
void free_cache(Cache* cache);
int cache_get(Cache* cache, uint32_t address);
void cache_put(Cache* cache, uint32_t address);

#endif
