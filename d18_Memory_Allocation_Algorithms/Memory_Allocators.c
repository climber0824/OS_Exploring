#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BLOCK_SIZE 256
#define NUM_BLOCKS 64
#define MAX_ORDER  10

typedef struct {
    void* memory;
    bool* is_allocated;
    size_t block_size;
    size_t num_blocks;
    size_t free_blocks;
} FixedAllocator;


typedef struct VariableBlock {
    size_t size;
    bool   is_allocated;
    struct VariableBlock* next;
    struct VariableBlock* prev;
} VariableBlock;


typedef struct {
    void* memory;
    VariableBlock* free_list;
    size_t total_size;
} VariableAllocator;


/*
The Buddy System recursively divides a large block of memory into 
smaller blocks until a block of the appropriate size is found.
*/
typedef struct {
    void* memory;
    void** free_lists;
    size_t min_block_size;
    size_t max_order;
} BuddyAllocator;


typedef struct {
    void* memory;
    size_t block_size;
    size_t num_blocks;
    size_t free_blocks;
    void** free_lists;
} MemoryPool;


typedef struct {
    size_t allocations;
    size_t deallocations;
    size_t faild_allocations;
    size_t total_allocated;
    size_t peak_usage;
    double fragmentation;
} AllocatorStats;


FixedAllocator* initFixedAllocator() {
    FixedAllocator* allocator = (FixedAllocator*)malloc(sizeof(FixedAllocator));
    
    allocator->memory = malloc(BLOCK_SIZE * NUM_BLOCKS);
    allocator->is_allocated = (bool*)calloc(NUM_BLOCKS, sizeof(bool));
    allocator->block_size = BLOCK_SIZE;
    allocator->num_blocks = NUM_BLOCKS;
    allocator->free_blocks = NUM_BLOCKS;

    return allocator;    
}


VariableAllocator* initVariableAllocator(size_t size) {
    VariableAllocator* allocator = (VariableAllocator*)malloc(
        sizeof(VariableAllocator)
    );

    allocator->memory = malloc(size);
    allocator->total_size = size;

    allocator->free_list = (VariableBlock*)allocator->memory;
    allocator->free_list->size = size - sizeof(VariableBlock);
    allocator->free_list->is_allocated = false;
    allocator->free_list->prev = NULL;
    allocator->free_list->next = NULL;

    return allocator;
}


BuddyAllocator* initBuddyAllocator(size_t min_block_size) {
    BuddyAllocator* allocator = (BuddyAllocator*)malloc(
        sizeof(BuddyAllocator)
    );

    allocator->min_block_size = min_block_size;
    allocator->max_order = MAX_ORDER;

    size_t total_size = min_block_size << MAX_ORDER;
    allocator->memory = malloc(total_size);

    allocator->free_lists = (void**)calloc(MAX_ORDER + 1, sizeof(void*));
    allocator->free_lists[MAX_ORDER] = allocator->memory;

    return allocator;
}


MemoryPool* initMemoryPool(size_t block_size, size_t num_blocks) {
    MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));

    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->free_blocks = num_blocks;

    pool->memory = malloc(block_size * num_blocks);
    pool->free_lists = (void**)malloc(sizeof(void*) * num_blocks);

    for (int i = 0; i < num_blocks; ++i) {
        pool->free_lists[i] = (char*)pool->memory + (i * block_size);
    }

    return pool;
}


void* allocateFixedBlock(FixedAllocator* allocator) {
    if (allocator->free_blocks == 0) {
        return NULL;
    }

    for (size_t i = 0; i < allocator->num_blocks; ++i) {
        if (!allocator->is_allocated[i]) {
            allocator->is_allocated[i] = true;
            allocator->free_blocks--;
            
            return (char*)allocator->memory + (i * allocator->block_size);
        }
    }
    
    return NULL;
}


void* allocateVariableBlock(VariableAllocator* allocator, size_t size) {
    VariableBlock* curr = allocator->free_list;
    VariableBlock* best_fit = NULL;
    size_t min_diff = allocator->total_size;

    // find best fit block
    while (curr != NULL) {
        if (!curr->is_allocated && curr->size >= size) {
            size_t diff = curr->size - size;
            if (diff < min_diff) {
                min_diff = diff;
                best_fit = curr;
            }
        }
        curr = curr->next;
    }

    if (best_fit == NULL) {
        return NULL;
    }

    // split block if possible
    if (best_fit->size > size + sizeof(VariableBlock) + 64) {
        VariableBlock* new_block = (VariableBlock*)((char*)best_fit + 
                                    sizeof(VariableBlock) + size);
        
        new_block->size = best_fit->size - size - sizeof(VariableBlock);
        new_block->is_allocated = false;
        new_block->next = best_fit->next;
        new_block->prev = best_fit;

        if (best_fit->next != NULL) {
            best_fit->next->prev = new_block;
        }

        best_fit->next = new_block;
        best_fit->size = size;
    }

    best_fit->is_allocated = true;
    
    return (char*)best_fit + sizeof(VariableBlock);
}


void freeFixedBlock(FixedAllocator* allocator, void* ptr) {
    if (ptr == NULL) return;

    size_t idx = ((char*)ptr - (char*)allocator->memory) / allocator->block_size;

    if (idx < allocator->num_blocks && allocator->is_allocated[idx]) {
        allocator->is_allocated[idx] = false;
        allocator->free_blocks++;
    }
}


// update allocator statistics
void updateStats(AllocatorStats* stats, size_t size, bool success) {
    if (success) {
        stats->allocations++;
        stats->total_allocated += size;
        if (stats->total_allocated > stats->peak_usage) {
            stats->peak_usage = stats->total_allocated;
        }        
    }
    else {
        stats->faild_allocations++;
    }
}


int main() {
    return 0;
}
