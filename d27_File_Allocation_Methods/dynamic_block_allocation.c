/*
Dynamic block allocation involves allocating and deallocating blocks on demand as files grow and shrink. 
This approach provides flexibility in managing storage space and helps optimize disk utilization over time.

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct DynamicAllocator {
    int *disk_blocks;
    int total_blocks;
    int free_blocks;
    
    struct {
        int *blocks;
        int count;
    } free_list;
    
    struct {
        int *blocks;
        int count;
    } recently_freed;   // for delayed reallocation
} DynamicAllocator;


DynamicAllocator *init_dynamic_allocator(int total_blocks) {
    DynamicAllocator *allocator = malloc(sizeof(DynamicAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = calloc(total_blocks, sizeof(int));
    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;

    allocator->free_list.blocks = malloc(total_blocks * sizeof(int));
    allocator->recently_freed.blocks = malloc(total_blocks * sizeof(int));

    if (!allocator->disk_blocks || !allocator->free_list.blocks || 
        !allocator->recently_freed.blocks) {
        if (allocator->disk_blocks) free(allocator->disk_blocks);
        if (allocator->free_list.blocks) free(allocator->free_list.blocks);
        if (allocator->recently_freed.blocks) free(allocator->recently_freed.blocks);
        free(allocator);

        return NULL;
    }

    // initialize free list
    for (int i = 0; i < total_blocks; i++) {
        allocator->free_list.blocks[i] = i;
    }

    allocator->free_list.count = total_blocks;
    allocator->recently_freed.count = 0;

    return allocator;    
}


// allocate blocks dynamically
int *allocate_blocks(DynamicAllocator *allocator, int count) {
    if (count > allocator->free_blocks) return NULL;

    int *allocated = malloc(count * sizeof(int));
    if (!allocated) return NULL;

    for (int i = 0; i < count; i++) {
        // prefer recently freed blocks for better locality
        if (allocator->recently_freed.count > 0) {
            allocated[i] = allocator->recently_freed.blocks[--allocator->recently_freed.count];
        }
        else {
            allocated[i] = allocator->free_list.blocks[--allocator->free_list.count];
        }
        allocator->disk_blocks[allocated[i]] = 1;
        allocator->free_blocks--;
    }

    return allocated;
}
