#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define CACHE_SIZE 256
#define BLOCK_SIZE 64
#define NUM_SETS   4


typedef struct {
    int valid;
    int tag;
    int data;
    int last_used;
} CacheLine;


typedef struct {
    CacheLine* lines;
    int num_sets;
    int blocks_per_set;
    int access_count;
    int hits;
    int misses;
} Cache;


Cache* initializeCache() {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->num_sets = NUM_SETS;
    cache->blocks_per_set = CACHE_SIZE / (BLOCK_SIZE * NUM_SETS);
    cache->lines = (CacheLine*)calloc(CACHE_SIZE / BLOCK_SIZE, sizeof(CacheLine));
    cache->access_count = 0;
    cache->hits = 0;
    cache->misses = 0;

    return cache;
}


int accessCache(Cache* cache, int address) {
    int set_idx = (address / BLOCK_SIZE) % cache->num_sets;
    int tag = address / (BLOCK_SIZE * cache->num_sets);
    int set_start = set_idx * cache->blocks_per_set;
    int found = 0;

    cache->access_count++;

    // look for hit
    for (int i = 0; i < cache->blocks_per_set; ++i) {
        int line_idx = set_start + 1;
        if (cache->lines[line_idx].valid && cache->lines[line_idx].tag == tag) {
            cache->hits++;
            cache->lines[line_idx].last_used = cache->access_count;
            found = 1;
            break;
        }
    }

    if (!found) {
        cache->misses++;

        // find LRU line
        int lru_idx = set_start;
        int lru_time = cache->lines[set_start].last_used;

        for (int i = 0; i < cache->blocks_per_set; ++i) {
            int line_idx = set_start + i;
            if (!cache->lines[line_idx].valid ||
                cache->lines[line_idx].last_used < lru_time) {
                    lru_idx = line_idx;
                    lru_time = cache->lines[line_idx].last_used;
                }
        }

        // replace line
        cache->lines[lru_idx].valid = 1;
        cache->lines[lru_idx].tag = tag;
        cache->lines[lru_idx].last_used = cache->access_count;
    }

    return found;
}


void printCacheStats(Cache* cache) {
    printf("\nCache Statistics:\n");
    printf("Total Accesses: %d\n", cache->access_count);
    printf("Cache Hits: %d\n", cache->hits);
    printf("Cache Misses: %d\n", cache->misses);
    printf("Hit Rate: %.2f%%\n", 
           (float)cache->hits / cache->access_count * 100);
}


int main() {
    Cache* cache = initializeCache();
    srand(time(NULL));
    int cnt = 1000;

    // simulate memory access
    for (int i = 0; i < cnt; ++i) {
        int address = rand() % (CACHE_SIZE * 4);    // larger address space
        accessCache(cache, address);
    }

    printCacheStats(cache);

    free(cache->lines);
    free(cache);

    return 0;
}

