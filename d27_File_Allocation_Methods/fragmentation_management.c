/*
Fragmentation management involves implementing strategies to prevent, detect, 
and handle both internal and external fragmentation. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct {
    int block_start;
    int block_len;
} FreeRegion;


typedef struct FragmentationManager {
    struct {
        int total_space;
        int used_space;
        int largest_free_block;
        double fragmentation_ratio;
    } metrics;

    FreeRegion *free_regions;

    int region_count;
    int total_blocks;
} FragmentationManager;


FragmentationManager *init_fragmentation_manager(int total_blocks) {
    FragmentationManager *manager = malloc(sizeof(FragmentationManager));
    if (!manager) return NULL;

    manager->free_regions = malloc(total_blocks * sizeof(*manager->free_regions));
    if (!manager->free_regions) {
        free(manager);

        return NULL;
    }

    manager->metrics.total_space = total_blocks;
    manager->metrics.used_space = 0;
    manager->metrics.largest_free_block = total_blocks;
    manager->metrics.fragmentation_ratio = 0.0;
    manager->region_count = 1;
    manager->total_blocks = total_blocks;

    // initialize with one free region
    manager->free_regions[0].block_start = 0;
    manager->free_regions[0].block_len = total_blocks;
    
    return manager;
}


// calculate fragmentation metrics
void update_fragmentation_metrics(FragmentationManager *manager) {
    int largest_free = 0;
    int total_free = 0;
    double weighted_fragmentation = 0.0;

    for (int i = 0; i < manager->region_count; i++) {
        int region_size = manager->free_regions[i].block_len;
        total_free += region_size;
        if (region_size > largest_free) {
            largest_free = region_size;
        }

        // weight smaller fragments more heavily
        weighted_fragmentation += (double)region_size / 
                                (double)manager->total_blocks *
                                (1.0 - (double)region_size / (double)manager->total_blocks);
    }

    manager->metrics.largest_free_block = largest_free;
    manager->metrics.used_space = manager->total_blocks - total_free;
    manager->metrics.fragmentation_ratio = weighted_fragmentation;
}


// defragmentation algorithm
void defragment(FragmentationManager *manager) {
    // sort free regions by start block
    for (int i = 0; i < manager->region_count - 1; i++) {
        for (int j = 0; j < manager->region_count - i - 1; j++)  {
            if (manager->free_regions[i].block_start > 
                manager->free_regions[j + 1].block_start) {
                // swap regions
                FreeRegion tmp;
                tmp = manager->free_regions[j];
                manager->free_regions[j] = manager->free_regions[j + 1];
                manager->free_regions[j + 1] = tmp;
            }
        }
    }

    // merge adjacent regions
    int write_idx = 0;
    for (int read_idx = 1; read_idx < manager->region_count; read_idx++) {
        if (manager->free_regions[write_idx].block_start +
            manager->free_regions[write_idx].block_len == 
            manager->free_regions[read_idx].block_start) {
            // merge regions
            manager->free_regions[write_idx].block_len += 
                manager->free_regions[read_idx].block_len;
        }
        else {
            // move to next region
            write_idx++;
            manager->free_regions[write_idx] = manager->free_regions[read_idx];
        }
    }

    manager->region_count = write_idx + 1;
}
