/*
Indexed allocation uses an index block to 
store all the pointers to the file’s data blocks.

The index block contains an array of disk block addresses, 
allowing direct access to any block of the file. 
This method supports both sequential and random access efficiently, 
though it has some space overhead for the index blocks.
*/

#include <stdlib.h>
#include <stdio.h>


#define BLOCK_SIZE 1024
#define INDEX_BLOCK_ENTRIES ((BLOCK_SIZE - sizeof(int)) / sizeof(int))


typedef struct IndexBlock {
    int num_entries;
    int block_pointers[INDEX_BLOCK_ENTRIES];    
} IndexBlock;


typedef struct IndexedFile {
    int idx_block;
    char name[256];
    size_t size;
} IndexedFile;


typedef struct IndexedAllocator {
    void *disk_blocks;
    IndexBlock *idx_blocks;
    int total_blocks;
    int free_blocks;
    int *free_list;
} IndexedAllocator;


IndexedAllocator *init_index_allocator(int total_blocks) {
    IndexedAllocator *allocator = malloc(sizeof(IndexedAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = malloc(total_blocks * BLOCK_SIZE);
    allocator->idx_blocks = malloc(total_blocks * sizeof(IndexBlock));
    allocator->free_list = malloc(total_blocks * sizeof(int));

    if (!allocator->disk_blocks || !allocator->idx_blocks || !allocator->free_list) {
        if (allocator->disk_blocks) free(allocator->disk_blocks);
        if (allocator->idx_blocks) free(allocator->idx_blocks);
        if (allocator->free_list) free(allocator->free_list);
        free(allocator);

        return NULL;
    }

    // initialize free list
    for (int i = 0; i < total_blocks; ++i) {
        allocator->free_list[i] = i;
        allocator->idx_blocks[i].num_entries = 0;
    }

    allocator->total_blocks = total_blocks;
    allocator->free_blocks = total_blocks;

    return allocator;
}


// allocate an indexed file
IndexedFile *create_indexed_file(IndexedAllocator *allocator, const char *name) {
    if (allocator->free_blocks == 0) return NULL;

    IndexedFile *file = malloc(sizeof(IndexedFile));
    if (!file) return NULL;

    // allocate index block
    file->idx_block = allocator->free_list[--allocator->free_blocks];
    strncpy(file->name, name, 255);
    file->size = 0;

    // initialize index block
    allocator->idx_blocks[file->idx_block].num_entries = 0;

    return file;
}
