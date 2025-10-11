#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define PROTECTION_NUM 100

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

typedef struct {
    size_t base_address;
    size_t limit;
    int    protection_bits;     // 1: read, 2: write, 3: execute
} MemoryProtection;

typedef struct {
    MemoryManager* manager;
    MemoryProtection* protection_table;
    int num_regions;
} ProtectedMemoryManager;

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

ProtectedMemoryManager* initProtectedMemoryManager(size_t size, int strategy) {
    ProtectedMemoryManager* pmm = (ProtectedMemoryManager*)malloc(
        sizeof(ProtectedMemoryManager)
    );
    pmm->manager = initMemoryManager(size, strategy);
    pmm->protection_table = (MemoryProtection*)malloc(
        sizeof(MemoryProtection) * PROTECTION_NUM
    );
    pmm->num_regions = 0;

    return pmm;
}

bool checkMemoryAccess(ProtectedMemoryManager* pmm,
                       size_t address,
                       int access_type) {

    for (int i = 0; i < pmm->num_regions; ++i) {
        MemoryProtection* mp = &pmm->protection_table[i];
        if (address >= mp->base_address &&
            address < mp->base_address + mp->limit) {
                return (mp->protection_bits & access_type) != 0;
        }
    }                        

    return false;
}

int main() {
    return 0;
}
