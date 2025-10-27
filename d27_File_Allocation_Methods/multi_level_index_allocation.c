/*
Multi-level index allocation extends the indexed allocation method 
by using multiple levels of index blocks. 
This approach allows for handling very large files efficiently 
while maintaining reasonable access times.

The primary index block contains pointers to 
secondary index blocks, which in turn point to data blocks. 
This hierarchical structure can be extended to multiple levels, 
similar to how modern file systems like ext4 implement their inode structure.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 1024
#define DIRECT_BLOCKS 12
#define INDIRECT_BLOCKS ((BLOCK_SIZE - sizeof(struct MultiLevelIndex)) / sizeof(int))


typedef struct MultiLevelIndex {
    int direct[DIRECT_BLOCKS];      // direct block pointers
    int single_indirect;            // single indirect block pointer
    int double_indirect;            // double indirect block pointer
    int triple_indirect;            // triple indirect block pointer
    size_t file_size;
} MultiLevelIndex;


typedef struct IndirectBlock {
    int blocks[INDIRECT_BLOCKS];
} IndirectBlock;


typedef struct MultiLevelAllocator {
    void *disk_blocks;
    MultiLevelIndex *idx_table;
    IndirectBlock *indirect_blocks;
    int total_blocks;
    int free_blocks;
    int *free_list;
} MultiLevelAllocator;


MultiLevelAllocator *init_multilevel_allocator(int total_blocks) {
    MultiLevelAllocator *allocator = malloc(sizeof(MultiLevelAllocator));
    if (!allocator) return NULL;

    allocator->disk_blocks = malloc(total_blocks * BLOCK_SIZE);
    allocator->idx_table = malloc(total_blocks * sizeof(MultiLevelIndex));
    allocator->indirect_blocks = malloc(total_blocks * sizeof(IndirectBlock));
    allocator->free_list = malloc(total_blocks * sizeof(int));
    allocator->total_blocks = total_blocks;
    allocator->free_blocks  = total_blocks;

    if (!allocator->disk_blocks || !allocator->idx_table 
        || !allocator->indirect_blocks || !allocator->free_list) {
        if (allocator->disk_blocks) free(allocator->disk_blocks);
        if (allocator->idx_table) free(allocator->idx_table);
        if (allocator->indirect_blocks) free(allocator->indirect_blocks);
        if (allocator->free_list) free(allocator->free_list);
        free(allocator);

        return NULL;
    }

    // initialize structures
    memset(allocator->idx_table, 0, total_blocks * sizeof(MultiLevelIndex));
    for (int i = 0; i < total_blocks; i++) {
        allocator->free_list[i] = i;
    }

    return allocator;
}


// get block address for given file offset
int get_block_address(MultiLevelAllocator *allocator, MultiLevelIndex *idx,
                      size_t block_num) {
    if (block_num < DIRECT_BLOCKS) {
        return idx->direct[block_num];
    }

    block_num -= DIRECT_BLOCKS;

    // single indirect
    if (block_num < INDIRECT_BLOCKS) {
        IndirectBlock *indirect = &allocator->indirect_blocks[idx->single_indirect];

        return indirect->blocks[block_num];
    }

    block_num -= INDIRECT_BLOCKS;

    // double indirect
    if (block_num < INDIRECT_BLOCKS * INDIRECT_BLOCKS) {
        int indirect_idx = block_num / INDIRECT_BLOCKS;
        int block_idx = block_num % INDIRECT_BLOCKS;

        IndirectBlock *double_indirect = &allocator->indirect_blocks[idx->double_indirect];
        IndirectBlock *indirect = &allocator->indirect_blocks[double_indirect->blocks[indirect_idx]];

        return indirect->blocks[block_idx];
    }

    block_num -= INDIRECT_BLOCKS;

    // triple indirect
    if (block_num < INDIRECT_BLOCKS * INDIRECT_BLOCKS * INDIRECT_BLOCKS) {
        int indirect_idx = block_num / INDIRECT_BLOCKS / INDIRECT_BLOCKS;
        int block_idx = block_num % INDIRECT_BLOCKS % INDIRECT_BLOCKS;

        IndirectBlock *triple_indirect = &allocator->indirect_blocks[idx->triple_indirect];
        IndirectBlock *indirect = &allocator->indirect_blocks[triple_indirect->blocks[indirect_idx]];
    
        return indirect->blocks[block_idx];
    }

    return -1;
}
