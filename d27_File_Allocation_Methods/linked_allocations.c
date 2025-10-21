/*
eliminates external fragmentation, 
suffers from poor random access performance 
and reliability issues, since a single corrupted pointer can break the chain.
*/

#include <stdlib.h>
#include <stdio.h>


#define BLOCK_SIZE 1024


typedef struct LinkedBlock {
    int block_num;
    int next_block;
    char data[BLOCK_SIZE];
} LinkedBlock;


typedef struct LinkedFile {
    int first_block;
    int last_block;
    char name[256];
    size_t size;
} LinkedFile;


typedef struct LinkedAllocator {
    LinkedBlock *disk_blocks;
    int total_blocks;
    int free_blocks;
    int *free_list;     // stack of free blocks
} LinkedAllocator;


LinkedAllocator* init_linked_allocator(int total_blocks) {
    LinkedAllocator *allocator = malloc(sizeof(LinkedAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = calloc(total_blocks, sizeof(LinkedBlock));
    allocator->free_list = malloc(total_blocks * sizeof(int));

    if (!allocator->disk_blocks || !allocator->free_list) {
        if (allocator->disk_blocks) free(allocator->disk_blocks);
        if (allocator->free_list) free(allocator->free_list);
        free(allocator);

        return NULL;
    }

    // initialize free list
    for (int i = 0; i < total_blocks; ++i) {
        allocator->free_list[i] = i;
        allocator->disk_blocks[i].next_block = -1;        
    }

    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;

    return allocator;
}


// allocate a new block and link it
int allocate_linked_block(LinkedAllocator *allocator, int previous_block) {
    if (allocator->free_blocks == 0) return -1;

    int new_block = allocator->free_list[--allocator->free_blocks];

    if (previous_block != -1) {
        allocator->disk_blocks[previous_block].next_block = new_block;
    }

    allocator->disk_blocks[new_block].block_num = new_block;
    allocator->disk_blocks[new_block].next_block = -1;

    return new_block;
}
