/*
Segmentation divides memory based on logical units, 
while paging divides memory into fixed-size pages.

1. Memory Fragmentation: Segmentation can lead to external fragmentation 
    as variable-sized segments are allocated and deallocated.
2. Address Translation: Address translation in segmentation is more complex than in paging.
3. Memory Utilization: Segmentation can provide better memory utilization than paging, 
    especially for programs with well-defined segments.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEGMENTS 256
#define MAX_MEMORY_SIZE 65536

#define READ_PERMISSION 4
#define WRITE_PERMISSION 2
#define EXEC_PERMISSION 1


typedef struct {
    unsigned int base_address;
    unsigned int limit;
    unsigned int protection_bits;
    unsigned char valid;
} SegmentTableEntry;


typedef struct {
    SegmentTableEntry entries[MAX_SEGMENTS];
    unsigned int size;
} SegmentTable;


typedef struct {
    char* memory;
    unsigned int size;
} PhysicalMemory;


// Real-world Applications
struct x86_segment_descriptor {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
};


SegmentTable* initSegmentTable() {
    SegmentTable* table = (SegmentTable*)malloc(sizeof(SegmentTable));
    table->size = 0;

    for (int i = 0; i < MAX_SEGMENTS; ++i) {
        table->entries[i].base_address = 0;
        table->entries[i].limit = 0;
        table->entries[i].protection_bits = 0;
        table->entries[i].valid = 0;
    }

    return table;
}


int createSegment(SegmentTable* table, unsigned int size, unsigned int protection) {
    if (table->size >= MAX_SEGMENTS) {
        printf("Error: Maximum segments reached\n");
        return -1;
    }

    int segment_num = table->size++;
    table->entries[segment_num].base_address = 0; // will be set during allocation
    table->entries[segment_num].limit = size;
    table->entries[segment_num].protection_bits = protection;
    table->entries[segment_num].valid = 1;

    return segment_num;
}


unsigned int translateAddress(SegmentTable* table, unsigned int segment, unsigned int offset) {
    if (table->size >= MAX_SEGMENTS) {
        printf("Error: Maximum segments reached\n");
        return -1;
    }

    SegmentTableEntry* entry = &table->entries[segment];

    if (!entry->valid) {
        printf("Error: Segment not in memory\n");
        return -1;
    }

    if (offset >= entry->limit) {
        printf("Error: Segment overflow\n");
        return -1;
    }

    return entry->base_address + offset;
}


int checkAccess(SegmentTable* table, unsigned int segment, unsigned int access_type) {
    if (segment >= table->size) return 0;

    SegmentTableEntry* entry = &table->entries[segment];
    
    return (entry->protection_bits & access_type) != 0;
}


PhysicalMemory* initPhysicalMemory() {
    PhysicalMemory* memory = (PhysicalMemory*)malloc(sizeof(PhysicalMemory));
    memory->size = MAX_MEMORY_SIZE;
    memory->memory = (char*)malloc(MAX_MEMORY_SIZE);

    return memory;
}


void shareSegment(SegmentTable* source, SegmentTable* target, 
                    int seg_num, int protection) {
    
    if (seg_num >= source->size) return;

    SegmentTableEntry* source_entry = &source->entries[seg_num];
    int new_seg = target->size++;

    target->entries[new_seg] = *source_entry;
    target->entries[new_seg].protection_bits = protection;
}


int allocateSegment(SegmentTable* table, PhysicalMemory* memory, int seg_num) {
    if (seg_num >= table->size) return -1;

    SegmentTableEntry* entry = &table->entries[seg_num];
    unsigned int size = entry->limit;

    // simple first-fit alloc
    unsigned int curr_addr = 0;

    while (curr_addr + size <= memory->size) {
        int space_available = 1;
        for (unsigned int i = 0; i < size; ++i) {
            if (memory->memory[curr_addr + i] != 0) {
                space_available = 0;
                break;
            }
        }

        if (space_available) {
            entry->base_address = curr_addr;
            return curr_addr;
        }

        curr_addr++;
    }
    
    return -1;
}


int main() {
    SegmentTable* table = initSegmentTable();
    PhysicalMemory* memory = initPhysicalMemory();

    int code_segment = createSegment(table, 1024, READ_PERMISSION | EXEC_PERMISSION);
    int data_segment = createSegment(table, 2048, READ_PERMISSION | WRITE_PERMISSION);
    int stack_segment = createSegment(table, 4096, READ_PERMISSION | WRITE_PERMISSION);

    printf("Code segment located at: %d\n",
            allocateSegment(table, memory, code_segment));
    printf("Data segment located at: %d\n",
            allocateSegment(table, memory, data_segment));
    printf("Stack segment located at: %d\n",
            allocateSegment(table, memory, stack_segment));            

    unsigned int logical_addr = 100;
    unsigned int physical_addr = translateAddress(table, data_segment, logical_addr);
    printf("Logical address %d in segment %d translate to physical address %d\n",
            logical_addr, data_segment, physical_addr);

    printf("Can write to data segment: %s\n",
            checkAccess(table, data_segment, WRITE_PERMISSION) ? "Yes" : "No");

    printf("Can execute data segment: %s\n",
            checkAccess(table, data_segment, EXEC_PERMISSION) ? "Yes" : "No");

    free(table);
    free(memory->memory);
    free(memory);

    return 0;
}


