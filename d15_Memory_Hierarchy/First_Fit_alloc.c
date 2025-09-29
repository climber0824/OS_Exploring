#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 1024
#define MIN_BLOCK_SIZE 16

// self-referential structs, you must use a tag name
// typedef struct MemoryBlock {
//     size_t size;
//     int is_allocatad;
//     struct MemoryBlock* next;
//     struct MemoryBlock* prev;
// } MemoryBlock;

// modern style:
struct MemoryBlock {
    size_t size;
    int is_allocatad;
    struct MemoryBlock* next;
    struct MemoryBlock* prev;
};

typedef struct MemoryBlock MemoryBlock;

// struct memory_block *p;  
// This avoids polluting the global namespace with typedefs and makes it obvious that it’s a struct.



// anonymous struct //
/*
typedef struct {
    size_t size;
    int is_allocatad;
    struct MemoryBlock* next;   // invalid
    struct MemoryBlock* prev;   // invalid
} MemoryBlock;
*/

typedef struct {
    void* memory;
    MemoryBlock* free_list;
    size_t total_size;
    size_t used_size;    
} MemoryManager;


// initialize memory manager
MemoryManager* initializeMemoryManager(size_t size) {
    MemoryManager* manager = (MemoryManager*)malloc(sizeof(MemoryManager));
    manager->memory = malloc(size);
    manager->total_size = size;
    manager->used_size = 0;

    // initialize free list with single block
    manager->free_list = (MemoryBlock*)manager->memory;
    manager->free_list->size = size - sizeof(MemoryBlock);
    manager->free_list->is_allocatad = 0;
    manager->free_list->next = NULL;
    manager->free_list->prev = NULL;

    return manager;
}


// allocate memory using First Fit
void* memoryAlloc(MemoryManager* manager, size_t size) {
    MemoryBlock* curr = manager->free_list;

    while (curr != NULL) {
        if (!curr->is_allocatad && curr->size >= size) {
            // split block if possible
            if (curr->size >= size + sizeof(MemoryBlock) + MIN_BLOCK_SIZE) {
                MemoryBlock* new_block = (MemoryBlock*)((char*)curr + sizeof(MemoryBlock) + size);
                new_block->size = curr->size - size - sizeof(MemoryBlock);
                new_block->is_allocatad = 0;
                new_block->next = curr->next;
                new_block->prev = curr;

                if (curr->next != NULL) {
                    curr->next->prev = new_block;
                }

                curr->next = new_block;
                curr->size = size;
            }

            curr->is_allocatad = 1;
            manager->used_size += curr->size + sizeof(MemoryBlock);

            return (void*)((char*)curr + sizeof(MemoryBlock));
        }
        curr = curr->next;
    }

    return NULL;
}


// free allocated memory
void memoryFree(MemoryManager* manager, void* ptr) {
    if (ptr == NULL) return;

    MemoryBlock* block = (MemoryBlock*)((char*)ptr - sizeof(MemoryBlock));
    block->is_allocatad = 0;
    manager->used_size -= (block->size + sizeof(MemoryBlock));

    // merge with next block if free
    if (block->next != NULL && !block->next->is_allocatad) {
        block->size += block->next->size + sizeof(MemoryBlock);
        block->next = block->next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }

    // merge with previous block if free
    if (block->prev != NULL && !block->prev->is_allocatad) {
        block->prev->size += block->size + sizeof(MemoryBlock);
        block->prev->next = block->next;
        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
    }
}


// print memory state
void printMemoryState(MemoryManager* manager) {
    MemoryBlock* curr = manager->free_list;
    int block_cnt = 0;

    printf("\nMemory state:\n");
    printf("Total size: %zu bytes\n", manager->total_size);
    printf("Used size: %zu bytes\n", manager->used_size);
    printf("Free size: %zu bytes\n", manager->total_size - manager->used_size);

    while (curr != NULL) {
        printf("Block %d: size=%zu, %s\n",
                block_cnt++,
                curr->size,
                curr->is_allocatad ? "Allocated" : "Free");
        curr = curr->next;
    }
}


int main() {
    MemoryManager* manager = initializeMemoryManager(MEMORY_SIZE);
    
    // test memory allocation and deallocation
    void* ptr1 = memoryAlloc(manager, 128);
    void* ptr2 = memoryAlloc(manager, 256);
    void* ptr3 = memoryAlloc(manager, 64);

    printMemoryState(manager);

    memoryFree(manager, ptr1);
    memoryFree(manager, ptr2);
    memoryFree(manager, ptr3);

    printMemoryState(manager);

    free(manager->memory);
    free(manager);
}

