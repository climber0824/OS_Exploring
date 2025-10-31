/*
Performance optimization in file allocation involves implementing strategies to improve read and write speeds 
while maintaining efficient space utilization. 
This includes techniques like prefetching, caching, and intelligent block placement.

The implementation must balance between different performance metrics 
such as sequential access speed, random access performance, and space efficiency. 
Modern systems often use predictive algorithms to optimize block placement and access patterns.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PREFETCH 256


typedef struct PerformanceOptimizer {
    struct {
        int *blocks;
        int count;
        int capacity;
    } prefetch_queue;

    struct {
        int *block_access_count;
        int *block_access_pattern;
        double *block_heat_map;
    } metrics;

    int total_blocks;
    int prefetch_window_size;
} PerformanceOptimizer;


int predict_next_blocks(PerformanceOptimizer *optimizer, int block_num, int predict_blocks[]);
void queue_prefetch(PerformanceOptimizer *optimizer, int predict_block);


PerformanceOptimizer *init_performance_optimizer(int total_blocks, int prefetch_size) {
    PerformanceOptimizer *optimizer = malloc(sizeof(PerformanceOptimizer));
    if (!optimizer) return NULL;

    optimizer->prefetch_queue.blocks = malloc(prefetch_size * sizeof(int));
    optimizer->metrics.block_access_count = calloc(total_blocks, sizeof(int));
    optimizer->metrics.block_access_pattern = calloc(total_blocks, sizeof(int));
    optimizer->metrics.block_heat_map = calloc(total_blocks, sizeof(double));

    if (!optimizer->prefetch_queue.blocks ||
        !optimizer->metrics.block_access_count ||
        !optimizer->metrics.block_access_pattern ||
        !optimizer->metrics.block_heat_map) {
        
        if (optimizer->prefetch_queue.blocks) free(optimizer->prefetch_queue.blocks);
        if (optimizer->metrics.block_access_count) free(optimizer->metrics.block_access_count);
        if (optimizer->metrics.block_access_pattern) free(optimizer->metrics.block_access_pattern);
        if (optimizer->metrics.block_heat_map) free(optimizer->metrics.block_heat_map);
        free(optimizer);

        return NULL;
    }

    optimizer->prefetch_queue.count = 0;
    optimizer->prefetch_queue.capacity = prefetch_size;
    optimizer->total_blocks = total_blocks;
    optimizer->prefetch_window_size = prefetch_size;

    return optimizer;
}


// update access patterns and trigger prefech
void update_access_pattern(PerformanceOptimizer *optimizer, int block_num) {
    // update access count
    optimizer->metrics.block_access_count[block_num]++;

    // update heat map with decay
    const double decay_fator = 0.95;

    for (int i = 0; i < optimizer->total_blocks; i++) {
        optimizer->metrics.block_heat_map[i] *= decay_fator;
    }
    optimizer->metrics.block_heat_map[block_num] += 1.0;

    // predict next blocks to prefech
    int predict_blocks[MAX_PREFETCH];
    int predict_count = predict_next_blocks(optimizer, block_num, predict_blocks);

    // queue prefetch operations
    for (int i = 0; i < predict_count; i++) {
        queue_prefetch(optimizer, predict_blocks[i]);
    }
}

// Uses a simple sequential prediction (block_num+1, +2, …) and adds a hot block from the heat map.
int predict_next_blocks(PerformanceOptimizer *optimizer, int block_num, int predict_blocks[]) {
    int count = 0;

    // predict sequential next few blocks
    for (int i = 1; i <= optimizer->prefetch_window_size; i++) {
        int next_block = block_num + i;
        if (next_block < optimizer->total_blocks) {
            predict_blocks[count++] = next_block;
        }
    }

    // optional: include a "hot" block based on heat map (skipping already predicted)
    double max_heat = 0.0;
    int hot_block = -1;
    for (int i = 0; i < optimizer->total_blocks; i++) {
        if (i == block_num) continue;
        if (optimizer->metrics.block_heat_map[i] > max_heat) {
            max_heat = optimizer->metrics.block_heat_map[i];
            hot_block = i;
        }
    }

    if (hot_block >= 0 && count < MAX_PREFETCH) {
        predict_blocks[count++] = hot_block;
    }

    return count;
}

// Keeps a FIFO prefetch queue of limited size (MAX_PREFETCH), dropping the oldest entries when full.
void queue_prefetch(PerformanceOptimizer *optimizer, int predict_block) {
    if (optimizer->prefetch_queue.count >= optimizer->prefetch_queue.capacity) {
        // drop oldest
        memmove(&optimizer->prefetch_queue.blocks[0],
                &optimizer->prefetch_queue.blocks[1],
                (optimizer->prefetch_queue.capacity - 1) * sizeof(int));
        optimizer->prefetch_queue.count = optimizer->prefetch_queue.capacity - 1;
    }

    optimizer->prefetch_queue.blocks[optimizer->prefetch_queue.count++] = predict_block;
}


// --- test driver ---
int main() {
    PerformanceOptimizer *opt = init_optimizer(100, 3);

    update_access_pattern(opt, 5);
    update_access_pattern(opt, 6);
    update_access_pattern(opt, 7);

    printf("Prefetch queue (%d): ", opt->prefetch_queue.count);
    for (int i = 0; i < opt->prefetch_queue.count; i++) {
        printf("%d ", opt->prefetch_queue.blocks[i]);
    }
    printf("\n");

    free(opt->prefetch_queue.blocks);
    free(opt->metrics.block_access_count);
    free(opt->metrics.block_access_pattern);
    free(opt->metrics.block_heat_map);
    free(opt);
}
