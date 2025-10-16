#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_TABLE_SIZE 1024
#define FRAME_SIZE 4096

typedef struct {
    int frame_num;
    unsigned int present : 1;
    unsigned int dirty : 1;
    unsigned int referenced : 1;
} PageTableEntry;


typedef struct {
    unsigned int* page_timestamps;
    unsigned int window_size;
} WorkingSetTracker;


typedef struct {
    PageTableEntry entries[PAGE_TABLE_SIZE];
} PageTable;


PageTable* initPageTable() {
    PageTable* pt = (PageTable*)malloc(sizeof(PageTable));
    if (pt == NULL) {
        perror("Failed to allocate memory for page table");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        pt->entries[i].frame_num = -1;        
        pt->entries[i].present = 0;
        pt->entries[i].dirty = 0;
        pt->entries[i].referenced = 0;
    }

    return pt;
}


int isInWorkingSet(WorkingSetTracker* tracker, unsigned int page, unsigned int curr_time) {
    return (curr_time - tracker->page_timestamps[page] <= tracker->window_size);
}


int getFrameNum(PageTable* pt, int page_num) {
    if (page_num < 0 || page_num >= PAGE_TABLE_SIZE) {
        fprintf(stderr, "Invalid page number: %d\n", page_num);
        return -1;
    }

    if (!pt->entries[page_num].present) {
        return -2;
    }
    return pt->entries[page_num].frame_num;
}


void setFrameNum(PageTable* pt, int page_num, int frame_num) {
    if (page_num < 0 || page_num >= PAGE_TABLE_SIZE) {
        fprintf(stderr, "Invalid page number: %d\n", page_num);
        return;
    }
    
    pt->entries[page_num].frame_num = frame_num;
    pt->entries[page_num].present = 1;    
}


int main() {
    PageTable* pt = initPageTable();

    setFrameNum(pt, 0, 5);
    setFrameNum(pt, 50, 10);

    int frame = getFrameNum(pt, 0);
    if (frame >= 0) {
        printf("Page 0 is in frame: %d\n", frame);
    }
    else if (frame == -2) {
        printf("Page fault for page 0\n");
    }

    frame = getFrameNum(pt, 2000);

    free(pt);

    return 0;
}
