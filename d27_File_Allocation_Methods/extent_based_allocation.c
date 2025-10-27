/*
Extent-based allocation is a modern approach that combines 
the benefits of contiguous allocation with the flexibility of other methods. 
An extent is a contiguous sequence of blocks that can be allocated as a unit.

This method reduces fragmentation while maintaining good performance characteristics. 
It’s particularly effective for large files and is used in many modern file systems like ext4 and XFS.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct Extent {
    int start_block;
    int length;
    struct Extent *next;
} Extent;


typedef struct ExtentFile {
    char name[256];
    Extent *extent_list;
    size_t size;
    int extent_count;
} ExtentFile;


typedef struct ExtentAllocator {
    int *disk_blocks;
    int total_blocks;
    int free_blocks;
    ExtentFile *files;
    int max_files;
} ExtentAllocator;


ExtentAllocator *init_extent_allocator(int total_blocks, int max_files) {
    ExtentAllocator *allocator = malloc(sizeof(ExtentAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = malloc(total_blocks * sizeof(int));
    allocator->files = calloc(max_files, sizeof(ExtentFile));

    if (!allocator->disk_blocks || !allocator->files) {
        if (allocator->disk_blocks) free(allocator->disk_blocks);
        if (allocator->files) free(allocator->files);
        free(allocator);

        return NULL;
    }

    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;
    allocator->max_files = max_files;

    return allocator;
}


// allocate new extent
Extent *allocate_extent(ExtentAllocator *allocator, int desired_length) {
    int curr_len = 0;
    int start_block = -1;

    // find contiguous free blocks
    for (int i = 0; i < allocator->total_blocks; i++) {
        if (allocator->disk_blocks[i] == 0) {
            if (curr_len == 0) start_block = i;
            curr_len++;

            if (curr_len == desired_length) {
                // allocate the extent
                Extent *extent = malloc(sizeof(Extent));
                if (!extent) return NULL;

                extent->start_block = start_block;
                extent->length = curr_len;
                extent->next = NULL;

                // mark blocks as allocated
                for (int j = start_block; j < start_block + curr_len; j++) {
                    allocator->disk_blocks[j] = 1;
                }
                allocator->free_blocks -= curr_len;

                return extent;
            }
        }
        else {
            curr_len = 0;
        }
    }

    return NULL;
}
