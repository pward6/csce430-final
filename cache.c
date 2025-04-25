#include <stdlib.h>
#include <string.h>
#include "cache.h"

static uint32_t get_index(uint64_t address){
    // 32 bit address is shifted 4 bits to cut off offset bits and then ANDed with 1000000 - 1 which is 0111111
    return (address >> OFFSET_BITS) & ((1 << INDEX_BITS) - 1);
}

static uint32_t get_tag(uint64_t address){
    return (address >> OFFSET_BITS) >> INDEX_BITS;
}

Cache* create_cache(){
    Cache* cache = malloc(sizeof(Cache));
    memset(cache, 0, sizeof(Cache)); // initialize all values to 0
    return cache;
}

void free_cache(Cache* cache){
    free(cache);
}

int cache_get(Cache* cache, uint64_t address){
    uint32_t tag = get_tag(address);
    uint32_t index = get_index(address);

    CacheBlock *main_line = &cache->main.blocks[index];

    // check main cache
    if (main_line->valid && main_line->tag == tag) {
        return 2; // hit
    }

    // check victim cache
    for (int i = 0; i < VICTIM_CACHE_SIZE; i++){
        CacheBlock *victim_block = &cache->victim.blocks[i];
        if (victim_block->valid && victim_block->tag == tag){
            CacheBlock temp = *main_line;
            *main_line = *victim_block;
            *victim_block = temp;
            return 1; // hit in victim block
        }
    }

    return 0; // miss
}

void cache_put(Cache* cache, uint64_t address){
    uint32_t tag = get_tag(address);
    uint32_t index = get_index(address);

    CacheBlock *main_line = &cache->main.blocks[index];
    if (main_line->valid){
        cache->victim.blocks[cache->victim.next_to_replace] = *main_line;
        cache->victim.next_to_replace = (cache->victim.next_to_replace + 1) % VICTIM_CACHE_SIZE;
    }

    main_line->tag = tag;
    main_line->valid = 1;
}