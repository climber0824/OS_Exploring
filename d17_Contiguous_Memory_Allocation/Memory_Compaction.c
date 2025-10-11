#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_MEMORY_SIZE 1024
#define MIN_PARTITION_SIZE 64

typedef struct MemoryBlock {
    size_t size;
    size_t start_address;
    bool is_allocated;
    struct MemoryBlock* next;
} MemoryBlock;

typedef struct {
    MemoryBlock* head;
    size_t total_size;
    size_t free_size;
    int allocation_strategy; // 1: First Fit, 2: Best Fit, 3: Worst Fit
} MemoryManager;

void compactMemory(MemoryManager* manager) {
    if (manager->head == NULL) return;

    MemoryBlock* curr = manager->head;
    size_t new_address = 0;

    while (curr != NULL) {
        if (curr->is_allocated) {
            // move block to new address
            curr->start_address = new_address;
            new_address += curr->size;
        }
        curr = curr->next;
    }

    // merge free blocks
    curr = manager->head;
    while (curr != NULL && curr->next != NULL) {
        if (!curr->is_allocated && !curr->next->is_allocated) {
            // merge blocks
            curr->size += curr->next->size;
            MemoryBlock* tmp = curr->next;
            curr->next = tmp->next;
            free(tmp);
        }
        else {
            curr = curr->next;
        }
    }
}

int main() {
    return 0;
}
