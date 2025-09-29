#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define CACHE_L1_SIZE 32
#define CACHE_L2_SIZE 256
#define MAIN_MEMORY_SIZE 1024
#define VIRTUAL_MEMORY_SIZE 4096


typedef struct {
    int data;
    int address;
    int valid;
    int access_time;
} MemoryBlock;

typedef struct {
    MemoryBlock l1_cache[CACHE_L1_SIZE];
    MemoryBlock l2_cache[CACHE_L2_SIZE];
    MemoryBlock main_memory[MAIN_MEMORY_SIZE];
    MemoryBlock virtual_memory[VIRTUAL_MEMORY_SIZE];

    int l1_hits;
    int l2_hits;
    int l1_misses;
    int l2_misses;
    int page_faults;
} MemoryHierarchy;


// Initialize memory hierarchy
void initializeMemory(MemoryHierarchy* mh) {
    // initialize l1 cache
    for (int i = 0; i < CACHE_L1_SIZE; ++i) {
        mh->l1_cache[i].valid = 0;
        mh->l1_cache[i].access_time = 10;   // 10 cycles
    }

    // initialize l2 cache
    for (int i = 0; i < CACHE_L2_SIZE; ++i) {
        mh->l2_cache[i].valid = 0;
        mh->l2_cache[i].access_time = 100;   // 100 cycles
    }

    // initialize  main memory
    for (int i = 0; i < MAIN_MEMORY_SIZE; ++i) {
        mh->main_memory[i].valid = 0;
        mh->main_memory[i].access_time = 100;   // 100 cycles
    }

    // initialize virtual memory
    for (int i = 0; i < VIRTUAL_MEMORY_SIZE; ++i) {
        mh->virtual_memory[i].valid = 0;
        mh->virtual_memory[i].access_time = 1000;   // 1000 cycles
    }

    mh->l1_hits = 0;
    mh->l1_misses = 0;
    mh->l2_hits = 0;
    mh->l2_misses = 0;
    mh->page_faults = 0;
}


// memory access function
int accessMemory(MemoryHierarchy* mh, int address, int data) {
    int total_time = 0;

    // check l1 cache
    int l1_idx = address % CACHE_L1_SIZE;
    if (mh->l1_cache[l1_idx].valid && mh->l1_cache[l1_idx].address == address) {
        mh->l1_hits++;
        total_time += mh->l1_cache[l1_idx].access_time;
        return total_time;
    }

    mh->l1_misses++;

    // check l2 cache
    int l2_idx = address % CACHE_L2_SIZE;
    if (mh->l2_cache[l2_idx].valid && mh->l2_cache[l2_idx].address == address) {
        mh->l2_hits++;
        
        // update l1 cache
        mh->l1_cache[l1_idx].data = mh->l2_cache[l2_idx].data;
        mh->l1_cache[l1_idx].address = address;
        mh->l1_cache[l1_idx].valid = 1;

        total_time += mh->l2_cache[l2_idx].access_time;
        return total_time;
    }

    mh->l2_misses++;

    // check main memory
    int mm_idx = address % MAIN_MEMORY_SIZE;
    if (mh->main_memory[mm_idx].valid && mh->main_memory[mm_idx].address == address) {
        // update l1 and l2 cache
        mh->l1_cache[l1_idx].data = mh->main_memory[mm_idx].data;
        mh->l1_cache[l1_idx].address = address;
        mh->l1_cache[l1_idx].valid = 1;

        mh->l2_cache[l1_idx].data = mh->main_memory[mm_idx].data;
        mh->l2_cache[l1_idx].address = address;
        mh->l2_cache[l1_idx].valid = 1;

        total_time += mh->main_memory[mm_idx].access_time;
        return total_time;
    }

    // page fault - access virtual memory
    mh->page_faults++;
    int vm_idx = address % VIRTUAL_MEMORY_SIZE;

    // update all levels of memory
    mh->main_memory[mm_idx].data = mh->virtual_memory[vm_idx].data;
    mh->main_memory[mm_idx].address = address;
    mh->main_memory[mm_idx].valid = 1;

    mh->l2_cache[l2_idx].data = mh->virtual_memory[vm_idx].data;
    mh->l2_cache[l2_idx].address = address;
    mh->l2_cache[l2_idx].valid = 1;

    mh->l1_cache[l1_idx].data = mh->virtual_memory[vm_idx].data;
    mh->l1_cache[l1_idx].address = address;
    mh->l1_cache[l1_idx].valid = 1;

    total_time += mh->virtual_memory[vm_idx].access_time;

    return total_time;    
}


// Print memory statistics
void printMemoryStats(MemoryHierarchy *mh) {
    printf("\nMemory Access Statistics:\n");
    printf("L1 Cache Hits: %d\n", mh->l1_hits);
    printf("L1 Cache Misses: %d\n", mh->l1_misses);
    printf("L2 Cache Hits: %d\n", mh->l2_hits);
    printf("L2 Cache Misses: %d\n", mh->l2_misses);
    printf("Page Faults: %d\n", mh->page_faults);
    
    float l1_hit_ratio = (float)mh->l1_hits / (mh->l1_hits + mh->l1_misses);
    float l2_hit_ratio = (float)mh->l2_hits / (mh->l2_hits + mh->l2_misses);
    
    printf("\nCache Performance:\n");
    printf("L1 Hit Ratio: %.2f%%\n", l1_hit_ratio * 100);
    printf("L2 Hit Ratio: %.2f%%\n", l2_hit_ratio * 100);
}


int main() {
    MemoryHierarchy mh;
    initializeMemory(&mh);

    // simulate memory access
    srand(time(NULL));
    int total_accesses = 1e3;
    int total_time = 0;

    for (int i = 0; i < total_accesses; ++i) {
        int address = rand() % VIRTUAL_MEMORY_SIZE;
        int data = rand() % 1000;
        total_time += accessMemory(&mh, address, data);
    }

    printMemoryStats(&mh);
    printf("\nTotal Access Time: %d cycles\n", total_time);
    printf("Average Access Time: %.2f cycles\n", (float)total_time / total_accesses);
    
    return 0;
}
