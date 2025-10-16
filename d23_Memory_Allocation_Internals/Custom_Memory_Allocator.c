#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define BLOCK_SIZE sizeof(block_t)
#define MAX_ALLOC 1000


// Optimize memory alignment for different architectures
#if defined(__x86_64__) || defined(_M_X64)
    #define ALIGNMENT 16
#else
    #define ALIGNMENT 8
#endif


static allocation_info_t allocations[MAX_ALLOC];
static int alloc_cnt = 0;


typedef struct block_t {
    bool is_free;           // whether block is free
    size_t size;            // size of block including header               
    struct block_t* next;   // next block in the list
    struct block_t* prev;   // previous block in the list
    char data[1];           // start of the data (flexible array number)
} block_t;


// Cache-friendly Block Placement
// Place frequently accessed metadata at the start of the block
// typedef struct block_t {
//     size_t size;          // Most frequently accessed
//     bool is_free;         // Second most frequently accessed
//     struct block_t* next; // Less frequently accessed
//     struct block_t* prev; // Less frequently accessed
//     char data[];         // Actual data
// } __attribute__((aligned(ALIGNMENT))) block_t;


typedef struct {
    block_t* free_list;
    block_t* used_list;
    size_t total_size;
    size_t used_size;
} heap_t;


typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
} allocation_info_t;


// global heap structure
static heap_t heap = {0};

void init_heap(size_t ini_size) {
    ini_size = ALIGN(ini_size);

    // request memory from OS
    void* memory = sbrk(ini_size);
    if (memory == (void*) - 1) {
        perror("Failed to initialize heap");
        return;
    }

    // initialize first block
    block_t* ini_block = (block_t*)memory;
    ini_block->size = ini_size;
    ini_block->is_free = true;
    ini_block->next = NULL;
    ini_block->prev = NULL;

    // initialize heap
    heap.free_list = ini_block;
    heap.used_size = NULL;
    heap.total_size = ini_size;
    heap.used_size = 0;
}


// find best fit block
block_t* find_best_fit(size_t size) {
    block_t* curr = heap.free_list;
    block_t* best = NULL;
    size_t smallest_diff = SIZE_MAX;

    while (curr != NULL) {
        if (curr->is_free && curr->size >= size) {
            size_t diff = curr->size - size;
            if (diff < smallest_diff) {
                smallest_diff = diff;
                best = curr;

                // if perfect fit, stop
                if (diff = 0) break;
            }
        }
        curr = curr->next;
    }

    return best;
}


void split_block(block_t* block, size_t size) {
    size_t remaining_size = block->size - size;

    // only split if remaining size is large enough for a new block
    if (remaining_size > BLOCK_SIZE + ALIGNMENT) {
        block_t* new_block = (block_t*)((char*)block + size);
        new_block->size = remaining_size;
        new_block->is_free = true;
        new_block->next = block->next;
        new_block->prev = block;

        if (block->next) {
            block->next->prev = new_block;
        }

        block->next = new_block;
        block->size = size;
    }
}


void* custom_malloc(size_t size) {
    if (size == 0) return NULL;

    // adjust size to include header and alignment
    size_t total_size = ALIGN(size + BLOCK_SIZE);

    // find suitable block
    block_t* block = find_best_fit(total_size);

    // if no suitable block found, request more memory
    if (block == NULL) {
        size_t request_size = total_size > 4096 ? total_size : 4096;
        void* memory = sbrk(request_size);
        if (memory == (void*)-1) {  // fail
            return NULL;
        }

        block = (block_t*)memory;
        block->size = request_size;
        block->is_free = true;
        block->next = heap.free_list;
        block->prev = NULL;

        if (heap.free_list) {
            heap.free_list->prev = block;
        }

        heap.free_list = block;
        heap.total_size += request_size;
    }

    // split block if necessary
    split_block(block, total_size);

    // mark block as used
    block->is_free = false;

    // remove from free-list and add to used-list
    if (block->prev) {
        block->prev->next = block->next;
    }
    else {
        heap.free_list = block->next;
    }

    if (block->next) {
        block->next->prev = block->prev;
    }

    block->next = heap.used_list;
    block->prev = NULL;

    if (heap.used_list) {
        heap.used_list->prev = block;
    }

    heap.used_list = block;
    heap.used_size += block->size;

    return block->data;
}


// coalesce adjacent free blocks
void coalesce_blocks(block_t* block) {
    // coalesce with next block
    if (block->next && block->next->is_free) {
        block->size += block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }

        // coalesce with previous block
        if (block->prev && block->prev->is_free) {
            block->prev->size += block->size;
            block->prev->next = block->next;
            if (block->next) {
                block->next->prev = block->prev;
            }
            block = block->prev;
        }
    }
}


void custom_free(void* ptr) {
    if (!ptr) return;

    // get block header
    block_t* block = (block_t*)((char*)ptr - BLOCK_SIZE);

    // mark block as free
    block->is_free = true;

    // remove from used-list
    if (block->prev) {
        block->prev->next = block->next;
    }
    else {
        heap.used_list = block->next;
    }

    if (block->next) {
        block->next->prev = block->prev;
    }

    block->next = heap.free_list;
    block->prev = NULL;
    if (heap.free_list) {
        heap.free_list->prev = block;
    }
    heap.free_list = block;

    heap.used_size -= block->size;

    // coalesce adjacent free blocks
    coalesce_blocks(block);
}


void print_memory_stats() {
    printf("\nMemory Statistics:\n");
    printf("Total Heap Size: %zu bytes\n", heap.total_size);
    printf("Used Size: %zu bytes\n", heap.used_size);
    printf("Free Size: %zu bytes\n", heap.total_size - heap.used_size);
    
    printf("\nFree Blocks:\n");

    block_t* curr = heap.free_list;
    while (curr) {
        printf("Block at %p, size: %zu\n", (void*)curr, curr->size);
        curr = curr->next;
    }

    printf("Used Blocks:\n");
    
    curr = heap.used_list;
    while (curr) {
        printf("Block at %p, size: %zu\n", (void*)curr, curr->size);
        curr = curr->next;
    }
}


void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = custom_malloc(size);
    if (ptr && alloc_cnt < MAX_ALLOC) {
        allocations[alloc_cnt].ptr = ptr;
        allocations[alloc_cnt].size = size;
        allocations[alloc_cnt].file = file;
        allocations[alloc_cnt].line = line;
        alloc_cnt++;
    }

    return ptr;
}


void debug_free(void* ptr, const char* file, int line) {
    for (int i = 0; i < alloc_cnt; ++i) {
        if (allocations[i].ptr == ptr) {
            memmove(&allocations[i], &allocations[i + 1], 
                    (alloc_cnt - i - 1) * sizeof(allocation_info_t));

            alloc_cnt--;
            break;                    
        }
    }
    custom_free(ptr);
}


void check_leaks() {
    if (alloc_cnt > 0) {
        printf("Memory Leaks Detected:\n");
        for (int i = 0; i < alloc_cnt; ++i) {
            printf("Leak: %zu bytes at %p, allocated in %s:%d\n",
                    allocations[i].size, allocations[i].ptr,
                    allocations[i].file, allocations[i].line);
        }
    }
    else {
        printf("No memory leaks detected\n");
    }
}


int main() {
    int size = 1024;
    init_heap(size * size);

    int* numbers = (int*)custom_malloc(10 * sizeof(int));
    char* string = (char*)custom_malloc(100);

    for (int i = 0; i < 10; ++i) {
        numbers[i] = i;
    }

    strcpy(string, "Hello, World!");

    print_memory_stats();
    
    custom_free(numbers);
    custom_free(string);

    check_leaks();

    return 0;
}
