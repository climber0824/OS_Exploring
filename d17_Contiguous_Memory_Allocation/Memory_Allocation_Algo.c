#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_MEMORY_SIZE 1024
#define MIN_PARTITION_SIZE 64


typedef struct MemoryBlock {
    size_t size;
    size_t start_address;
    bool   is_allocated;
    struct MemoryBlock* next;
} MemoryBlock;


typedef struct {
    MemoryBlock* head;
    size_t       total_size;
    size_t       free_size;
    int          allocated_strategy;    // 1: First fit, 2: Best fit, 3: Worst fit
} MemoryManager;


MemoryManager* initMemoryManager(size_t size, int strategy) {
    MemoryManager* manager = (MemoryManager*)malloc(sizeof(MemoryManager));
    manager->total_size = size;
    manager->free_size = size;
    manager->allocated_strategy = strategy;

    // create initial free block
    manager->head = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    manager->head->size = size;
    manager->head->start_address = 0;
    manager->head->is_allocated = false;
    manager->head->next = NULL;

    return manager;
}


void freeMemoryManager(MemoryManager* manager) {
    if (manager->head->next != NULL) {
        free(manager->head->next);
    }
    if (manager->head != NULL) {
        free(manager->head);
    }
    if (manager != NULL) {
        free(manager);
    }
}


// First fit algorithm
MemoryBlock* firstFit(MemoryManager* manager, size_t size) {
    MemoryBlock* curr = manager->head;

    while (curr != NULL) {
        if (!curr->is_allocated && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}


// Best fit algo
MemoryBlock* bestFit(MemoryManager* manager, size_t size) {
    MemoryBlock* curr = manager->head;
    MemoryBlock* best_block = NULL;
    size_t smallest_diff = manager->total_size;

    while (curr != NULL) {
        if (!curr->is_allocated && curr->size >= size) {
            size_t diff = curr->size - size;
            if (diff < smallest_diff) {
                smallest_diff = diff;
                best_block = curr;
            }
        }
        curr = curr->next;
    }

    return best_block;
}


// Worst fit algo
MemoryBlock* worstFit(MemoryManager* manager, size_t size) {
    MemoryBlock* curr = manager->head;
    MemoryBlock* worst_block = NULL;
    size_t       largest_diff = 0;

    while (curr != NULL) {
        if (!curr->is_allocated && curr->size >= size) {
            size_t diff = curr->size - size;
            if (diff > largest_diff) {
                largest_diff = diff;
                worst_block = curr;
            }
        }
        
        curr = curr->next;
    }

    return worst_block;
}


void* allocMemory(MemoryManager* manager, size_t size) {
    if (size < MIN_PARTITION_SIZE || size > manager->free_size) {
        printf("size error: %ld", size);
        return NULL;
    } 

    MemoryBlock* selected_block = NULL;
    printf("strategy: %d\n", manager->allocated_strategy);
    switch(manager->allocated_strategy) {
        case 1:
            selected_block = firstFit(manager, size);
            printf("case 1\n");
            break;

        case 2:
            selected_block = bestFit(manager, size);
            printf("case 2\n");
            break;

        case 3:
            selected_block = worstFit(manager, size);
            printf("case 3\n");
            break;

        default:
            selected_block = firstFit(manager, size);
            break;
    }

    if (selected_block == NULL) {
        printf("NULL");
        return NULL;
    }
    else {
        printf("selected block size: %ld\n", selected_block->size);
    }
    if (selected_block->size > size + MIN_PARTITION_SIZE) {
        MemoryBlock* new_block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
        new_block->size = selected_block->size - size;
        new_block->start_address = selected_block->start_address + size;
        new_block->is_allocated = false;
        new_block->next = selected_block->next;

        selected_block->size = size;
        selected_block->next = new_block;
    }

    selected_block->is_allocated = true;
    manager->free_size -= selected_block->size;

    return (void*)selected_block->start_address;
}


int main() {
    MemoryManager* manager1 = initMemoryManager(MAX_MEMORY_SIZE, 1);
    MemoryManager* manager2 = initMemoryManager(MAX_MEMORY_SIZE, 2);
    MemoryManager* manager3 = initMemoryManager(MAX_MEMORY_SIZE, 3);

    unsigned seed;
    seed = (unsigned)time(NULL); 
    srand(seed); 
    int memory_size = (rand() % (MAX_MEMORY_SIZE / 4)) + MIN_PARTITION_SIZE;
    printf("Attempting to allocate a block of size: %d\n\n", memory_size);


    void* addr1 = allocMemory(manager1, memory_size);
    void* addr2 = allocMemory(manager2, memory_size);
    void* addr3 = allocMemory(manager3, memory_size);
    
    if (addr1 != NULL) {        
        printf("First Fit allocated at address: %zu\n", (size_t)addr1);
    }
    
    if (addr2 != NULL) {
        printf("Best Fit allocated at address:  %zu\n", (size_t)addr2);
    }
    
    if (addr3 != NULL) {
        printf("Worst Fit allocated at address: %zu\n", (size_t)addr3);
    }

    // free the managers and their internal blocks
    freeMemoryManager(manager1);
    freeMemoryManager(manager2);
    freeMemoryManager(manager3);
    free(addr1);
    free(addr2);
    free(addr3);

    return 0;
}
