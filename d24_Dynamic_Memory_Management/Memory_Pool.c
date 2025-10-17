#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define POOL_BLOCK_SIZE 64
#define POOL_BLOCK_COUNT 1024


typedef struct MemoryPool {
    void* start;
    void* free_list;
    size_t block_size;
    size_t total_blocks;
    size_t free_blocks;
} MemoryPool;


typedef struct AdvancedMemoryPool {
    void* start;
    void* free_list;
    size_t block_size;
    size_t total_blocks;
    size_t free_blocks;
    uint32_t alignment;
    uint32_t flags;
    void (*cleanup_callback)(void*);
} AdvancedMemoryPool;


static size_t align_size(size_t size, size_t alignment) {
    return (size + (alignment - 1)) & !(alignment - 1);
    // equals to: size_t aligned_size = ((size + alignment - 1) / alignment) * alignment;
}


MemoryPool* pool_init(size_t block_size, size_t block_count) {
    MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));
    if (!pool) return NULL;

    pool->start = malloc(block_size * block_count);
    if (!pool->start) {
        free(pool);
        return NULL;
    }

    pool->block_size = block_size;
    pool->total_blocks = block_count;
    pool->free_blocks = block_count;

    char* block = (char*)pool->start;
    pool->free_list = block;

    for (size_t i = 0; i < block_count - 1; ++i) {
        *(void**)(block) = block + block_size;  // writes the address of the next block at the start of the current block.
        block += block_size;
    }

    *(void**)(block) = NULL;    // last block points to NULL

    return pool;
}


void* pool_alloc(MemoryPool* pool) {
    if (!pool || !pool->free_blocks) return NULL;

    void* block = pool->free_list;
    pool->free_list = *(void**)block;   // block contains, at its start, a pointer to the next free block.
    pool->free_blocks--;

    return block;
}


// return block to pool
void pool_free(MemoryPool* pool, void* block) {
    if (!pool || !block) return;

    *(void**)block = pool->free_list;
    pool->free_list = block;
    pool->free_blocks++;
}


void pool_destroy(MemoryPool* pool) {
    if (!pool) return;
    free(pool->start);
    free(pool->free_list);
    free(pool);
}


AdvancedMemoryPool* advanced_pool_create(
    size_t block_size,
    size_t block_count,
    size_t alignment
) {
    AdvancedMemoryPool* pool = malloc(sizeof(AdvancedMemoryPool));
    // AdvancedMemoryPool* pool = (AdvancedMemoryPool*)malloc(sizeof(AdvancedMemoryPool));
    
    if (!pool) return NULL;

    size_t aligned_size = align_size(block_size, alignment);
    
    void* memory;
    if (posix_memalign(&memory, alignment, aligned_size * block_count) != 0) {
        free(pool);
        return NULL;
    }

    pool->start = memory;
    pool->block_size = aligned_size;
    pool->total_blocks = block_count;
    pool->free_blocks = block_count;
    pool->alignment = alignment;

    char *block = (char*)pool->start;
    pool->free_list = block;

    for (size_t i = 0; i < block_count - 1; ++i) {
        *(void**)(block) = block + aligned_size;
        block += aligned_size;
    }
    *(void**)(block) = NULL;

    return pool;
}

int main() {
    MemoryPool* pool = pool_init(POOL_BLOCK_SIZE, POOL_BLOCK_COUNT);
    AdvancedMemoryPool* advanced_pool = advanced_pool_create(POOL_BLOCK_SIZE, POOL_BLOCK_COUNT, 8);

    if (!pool) {
        printf("Failed to create memory pool\n");
        return 1;
    }
    if (!advanced_pool) {
        printf("Failed to create advanced memory pool\n");
        return 1;
    }

    void* blocks[5], *advanced_blocks[5];
    
    for (int i = 0; i < 5; ++i) {
        blocks[i] = pool_alloc(pool);
        advanced_blocks[i] = pool_alloc(advanced_pool);
        if (blocks[i]) {
            printf("Allocated block %d at %p\n", i, blocks[i]);
        }
        if (advanced_blocks[i]) {
            printf("Allocated advanced block %d at %p\n", i, advanced_blocks[i]);
        }

    }

    for (int i = 0; i < 5; ++i) {
        pool_free(pool, blocks[i]);
        printf("Free block %d\n", i);

        pool_free(advanced_pool, advanced_blocks[i]);
        printf("Free advanced block %d\n", i);
    }

    pool_destroy(pool);
    pool_destroy(advanced_pool);
    
    return 0;
}


