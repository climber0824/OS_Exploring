/*
a hybrid scheme might use extent-based allocation for large files, 
direct blocks for small files, and indirect blocks for medium-sized files. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "multi_level_index_allocation.c"
#include "extent_based_allocation.c"


#define DIRECT_BLOCKS 12
#define MAX_FILES 512


typedef enum AllocationType {
    ALLOCATION_DIRECT,
    ALLOCATION_INDIRECT,
    ALLOCATION_EXTENT,
    ALLOCATION_MULTILEVEL
} AllocationType;


typedef struct HybridBlock {
    union {
        int direct_blocks[DIRECT_BLOCKS];
        MultiLevelIndex multilevel_idx;
        Extent *extent_list;
        int *indirect_blocks;
    } data;
    AllocationType type;
} HybridBlock;


typedef struct HybridFile {
    char name[256];
    size_t size;
    HybridBlock block;
} HybridFile;


typedef struct HybridAllocator {
    void *disk_blocks;
    int total_blocks;
    int free_blocks;
    HybridFile *files;
    int max_files;
    size_t direct_threshold;    // Size threshold for direct allocation
    size_t extent_threshold;    // Size threshold for extent allocation
} HybridAllocator;


// choose appropriate allocation method based on file size
AllocationType determine_allocation_type(HybridAllocator *allocator, size_t file_size) {
    if (file_size <= allocator->direct_threshold) {
        return ALLOCATION_DIRECT;
    }
    else if (file_size <= allocator->extent_threshold) {
        return ALLOCATION_EXTENT;
    }
    else {
        return ALLOCATION_MULTILEVEL;
    }
}


HybridAllocator *init_hybrid_allocator(int total_blocks, size_t block_size, size_t direct_threshold, size_t extent_threshold) {
    HybridAllocator *allocator = malloc(sizeof(HybridAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = calloc(total_blocks, block_size);
    if (!allocator->disk_blocks) {
        fprintf(stderr, "Failed to allocate disk block\n");
        free(allocator);
        
        return NULL;
    }

    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;

    allocator->files = calloc(MAX_FILES, sizeof(HybridFile));
    if (!allocator->files) {
        fprintf(stderr, "Failed to allocate file table\n");
        free(allocator->disk_blocks);
        free(allocator);

        return NULL;
    }

    allocator->max_files = MAX_FILES;
    allocator->direct_threshold = DIRECT_BLOCKS * block_size;
    allocator->extent_threshold = allocator->direct_threshold * 4;

    return allocator;
}


// create new file with hybrid allocation
HybridFile *create_hybrid_file(HybridAllocator *allocator,
                                const char *name,
                                size_t initial_size) {

    if (allocator->free_blocks == 0) return NULL;

    HybridFile *file = malloc(sizeof(HybridFile));
    if (!file) return NULL;

    strncpy(file->name, name, 255);
    file->size = initial_size;

    AllocationType type = determine_allocation_type(allocator, initial_size);
    file->block.type = type;

    // initialize based on allocation type
    switch (type) {
        case ALLOCATION_DIRECT:
            memset(file->block.data.direct_blocks, 0,
                    sizeof(file->block.data.direct_blocks));
            break;

        case ALLOCATION_EXTENT:
            file->block.data.extent_list = 
                allocate_extent(allocator,
                            (initial_size + BLOCK_SIZE - 1) / BLOCK_SIZE);
            break;

        case ALLOCATION_MULTILEVEL:
            memset(&file->block.data.multilevel_idx, 0,
                        sizeof(MultiLevelIndex));
            break;
    }

    return file;
}
