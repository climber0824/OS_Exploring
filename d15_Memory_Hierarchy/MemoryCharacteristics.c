#include <stdio.h>
#include <stdlib.h>

#define REGISTER_SIZE 64
#define CACHE_LINE_SIZE 64
#define PAGE_SIZE 4096

typedef struct {
    int size_bits;
    int access_time_cycles;
    float cost_per_byte;
    int volatile_storage;
    char *type_name;
} MemoryCharacteristics;

void printMemoryCharacteristics(MemoryCharacteristics *mc) {
    printf("Memory Type: %s\n", mc->type_name);
    printf("Size (bits): %d\n", mc->size_bits);
    printf("Access Time (cycles): %d\n", mc->access_time_cycles);
    printf("Cost per byte: $%.2f\n", mc->cost_per_byte);
    printf("Volatile: %s\n\n", mc->volatile_storage ? "Yes" : "No");
}

int main() {
    MemoryCharacteristics memories[] = {
        {REGISTER_SIZE, 1, 100.0, 1, "CPU Register"},
        {CACHE_LINE_SIZE, 3, 50.0, 1, "L1 Cache"},
        {CACHE_LINE_SIZE * 8, 10, 25.0, 1, "L2 Cache"},
        {PAGE_SIZE, 100, 1.0, 1, "Main Memory"},
        {PAGE_SIZE * 1024, 10000, 0.1, 0, "Virtual Memory"}
    };
    
    int num_memories = sizeof(memories) / sizeof(MemoryCharacteristics);
    
    for(int i = 0; i < num_memories; i++) {
        printMemoryCharacteristics(&memories[i]);
    }
    
    return 0;
}
