#include <stdlib.h>
#include <stdio.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 1024
#define FRAME_SIZE PAGE_SIZE
#define NUM_FRAMES 256


typedef struct {
    int valid;
    int frame_num;
    int ref;
    int modified;
} PageTableEntry;


typedef struct {
    PageTableEntry* entries;
    int num_pages;
} PageTable;


typedef struct {
    void* memory;
    int num_frames;
    int* frame_status;  // 0 -> free, 1 -> used    
} PhysicalMemory;


PageTable* initializePageTable() {
    PageTable* pt = (PageTable*)malloc(sizeof(PageTable));
    pt->num_pages = NUM_PAGES;
    pt->entries = (PageTableEntry*)calloc(NUM_PAGES, sizeof(PageTableEntry));
    
    return pt;
}


PhysicalMemory* initializePhysicalMemory() {
    PhysicalMemory* pm = (PhysicalMemory*)malloc(sizeof(PhysicalMemory));
    pm->num_frames = NUM_FRAMES;
    pm->memory = malloc(NUM_FRAMES * FRAME_SIZE);
    pm->frame_status = (int*)calloc(NUM_FRAMES, sizeof(int));

    return pm;
}


int allocateFrame(PhysicalMemory* pm) {
    for (int i = 0; i < pm->num_frames; ++i) {
        if (pm->frame_status[i] == 0) {
            pm->frame_status[i] = 1;
            return i;
        }
    }
    return -1;  // no free frames
}


void* accessMemory(PageTable* pt, PhysicalMemory* pm, int virtual_address) {
    int page_num = virtual_address / PAGE_SIZE;
    int offset = virtual_address % PAGE_SIZE;

    if (page_num >= pt->num_pages) {
        printf("Invalid virtual address\n");
        return NULL;
    }

    PageTableEntry* pte = &pt->entries[page_num];

    if (!pte->valid) {
        // page fault
        int frame = allocateFrame(pm);
        if (frame == -1) {
            printf("No free frames\n");
            return NULL;
        }

        pte->valid = 1;
        pte->frame_num = frame;
        pte->ref = 1;
        printf("Page fault handle: page %d -> frame %d\n", page_num, frame);
    }

    return (char*)pm->memory + (pte->frame_num * FRAME_SIZE) + offset;
}


int main() {
    PageTable* pt = initializePageTable();
    PhysicalMemory* pm = initializePhysicalMemory();
    int cnt = 100;

    // test memory access
    for (int i = 0; i < cnt; ++i) {
        int virtual_address = rand() % (NUM_PAGES * PAGE_SIZE);
        void* physical_address = accessMemory(pt, pm, virtual_address);

        if (physical_address != NULL) {
            printf("Virtual address: 0x%x -> Physical address: %p\n", 
                    virtual_address, physical_address);
        }        
    }

    free(pt->entries);
    free(pt);
    free(pm->memory);
    free(pm->frame_status);
    free(pm);

    return 0;
}
