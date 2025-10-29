/*
 Block size optimization involves choosing the appropriate block size 
 for the file system and implementing mechanisms to handle different block sizes efficiently. 
 The choice of block size affects both storage efficiency and performance.
 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct BlockSizeManager {
    int standard_block_size;
    int sub_block_size;
    int blocks_per_group;

    struct {
        void *data;
        int *bitmap;
        int total_sub_blocks;
        int free_sub_blocks;
    } *sub_block_groups;

    int group_count;
} BlockSizeManager;


BlockSizeManager *init_block_size_manager(int std_size, int sub_size, int groups) {
    BlockSizeManager *manager = malloc(sizeof(BlockSizeManager));
    if (!manager) return NULL;

    manager->standard_block_size = std_size;
    manager->sub_block_size = sub_size;
    manager->blocks_per_group = std_size / sub_size;
    manager->group_count = groups;

    manager->sub_block_groups = malloc(groups * sizeof(*manager->sub_block_groups));
    if (!manager->sub_block_groups) {
        free(manager);

        return NULL;
    }

    // initialize each group
    for (int i = 0; i < groups; ++i) {
        manager->sub_block_groups[i].data = malloc(std_size);
        manager->sub_block_groups[i].bitmap = calloc(manager->blocks_per_group, sizeof(int));
        manager->sub_block_groups[i].total_sub_blocks = manager->blocks_per_group;
        manager->sub_block_groups[i].free_sub_blocks = manager->blocks_per_group;

        if (!manager->sub_block_groups[i].data ||
            !manager->sub_block_groups[i].bitmap) {
            // cleanup on failure
            for (int j = 0; j < i; ++j) {
                free(manager->sub_block_groups[j].data);
                free(manager->sub_block_groups[j].bitmap);
            }
            free(manager->sub_block_groups);
            free(manager);

            return NULL;
        }
    }

    return manager;
}


// allocate appropriate block size
void *allocate_optimal_block(BlockSizeManager *manager, size_t size) {
    if (size <= manager->sub_block_size) {
        
        // allocate sub block
        for (int i = 0; i < manager->group_count; ++i) {
            if (manager->sub_block_groups[i].free_sub_blocks > 0) {

                // find free sub block
                for (int j = 0; j < manager->blocks_per_group; ++j) {
                    if (!manager->sub_block_groups[i].bitmap[j]) {
                        manager->sub_block_groups[i].bitmap[j] = 1;
                        manager->sub_block_groups[i].free_sub_blocks--;

                        return (char*)manager->sub_block_groups[i].data + 
                                (j * manager->sub_block_size);
                    }
                }
            }
        }
    }

    // allocate standard block
    return malloc(manager->standard_block_size);
}
