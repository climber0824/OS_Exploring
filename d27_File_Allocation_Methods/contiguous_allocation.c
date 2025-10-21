#include <stdlib.h>
#include <stdio.h>



typedef struct ContiguousFile {
    int start_block;    // starting block number
    int len;            // number of blocks
    char name[256];
    size_t size;
} ContiguousFile;


typedef struct ContiguousAllocator {
    int *disk_blocks;   // arrary representing disk blocks
    int total_blocks;
    int free_blocks;
} ContiguousAllocator;


ContiguousAllocator *init_contiguous_allocator(int total_blocks) {
    ContiguousAllocator *allocator = malloc(sizeof(ContiguousAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = calloc(total_blocks, sizeof(int));
    if (!allocator->disk_blocks) {
        free(allocator);
        return NULL;
    }

    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;

    return allocator;
}


// allocate contiguous blocks
int allocate_contiguous(ContiguousAllocator *allocator, int blocks_needed) {
    int curr_count = 0;
    int start_block = -1;

    for (int i = 0; i < allocator->total_blocks; ++i) {
        if (allocator->disk_blocks[i] == 0) {
            if (curr_count == 0) start_block = i;
            curr_count++;

            if (curr_count == blocks_needed) {
                // mark blocks as allocated
                for (int j = start_block; j < start_block + blocks_needed; ++j) {
                    allocator->disk_blocks[j] = 1;                    
                }
                allocator->free_blocks -= blocks_needed;
                
                return start_block;
            }
        }
        else {
            curr_count = 0;
        }
    }

    return -1;  // not enough contiguous space
}
