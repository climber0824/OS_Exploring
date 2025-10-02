#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 1024
#define PAGE_SIZE 4096
#define OFFSET_BITS 12
#define PAGE_NUMBER_BITS 10


typedef struct {
    unsigned int page_num;
    unsigned int frame_num;
    unsigned int valid;
    unsigned int access_time;
} TLBEntry;


typedef struct {
    unsigned int frame_num;
    unsigned int valid;
    unsigned int protection_bits;
} PageTableEntry;

typedef struct {
    TLBEntry* tlb;
    PageTableEntry* page_table;
    unsigned int tlb_size;
    unsigned int page_table_size;
    unsigned int clock;
} AddressTranslator;


AddressTranslator* initializeAddressTranslator() {
    AddressTranslator* at = (AddressTranslator*)malloc(sizeof(AddressTranslator));

    // initialize TLB
    at->tlb = (TLBEntry*)calloc(TLB_SIZE, sizeof(TLBEntry));
    at->tlb_size = TLB_SIZE;

    // initialize page table
    at->page_table = (PageTableEntry*)calloc(PAGE_TABLE_SIZE, sizeof(PageTableEntry));
    at->page_table_size = PAGE_TABLE_SIZE;

    at->clock = 0;

    return at;
}


// update TLB using LRU replacement
void updateTLB(AddressTranslator* at, unsigned int page_num, unsigned int frame_num) {
    int lru_idx = 0;
    unsigned int min_time = at->tlb[0].access_time;

    // find LRU entry
    for (int i = 1; i < at->tlb_size; ++i) {
        if (at->tlb[i].access_time < min_time) {
            min_time = at->tlb[i].access_time;
            lru_idx = i;
        }
    }

    // update TLB entry
    at->tlb[lru_idx].page_num = page_num;
    at->tlb[lru_idx].frame_num = frame_num;
    at->tlb[lru_idx].valid = 1;
    at->tlb[lru_idx].access_time = at->clock++;
}


// translate logical to physical address
unsigned int translateAddress(AddressTranslator* at, unsigned int logical_address) {
    unsigned int page_num = logical_address >> OFFSET_BITS;
    unsigned int offset = logical_address & ((1 << OFFSET_BITS) - 1);
    unsigned int frame_num = 0;
    bool tlb_hit = false;

    // check TLB first
    for (int i = 0; i < at->tlb_size; ++i) {
        if (at->tlb[i].valid && at->tlb[i].page_num == page_num) {
            frame_num = at->tlb[i].frame_num;
            at->tlb[i].access_time = at->clock++;
            tlb_hit = true;
            break;
        }
    }

    // if TLB miss, check page table
    if (!tlb_hit) {
        if (page_num >= at->page_table_size) {
            printf("Error: page number out of bound\n");
            return -1;
        }

        if (!at->page_table[page_num].valid) {
            printf("Error: page fault for page number %u\n", page_num);
            return -1;
        }

        frame_num = at->page_table[page_num].frame_num;
        updateTLB(at, page_num, frame_num);
    }

    return (frame_num << OFFSET_BITS) | offset;
}


void printTranslationStats(AddressTranslator* at) {
    printf("\nAddress Translation Statistics:\n");
    printf("TLB Size: %u entries\n", at->tlb_size);
    printf("Page Table Size: %u entries\n", at->page_table_size);
    printf("Page Size: %u bytes\n", PAGE_SIZE);
    printf("Valid TLB Entries:\n");
    
    for(int i = 0; i < at->tlb_size; i++) {
        if(at->tlb[i].valid) {
            printf("Page %u -> Frame %u (Access Time: %u)\n",
                   at->tlb[i].page_num,
                   at->tlb[i].frame_num,
                   at->tlb[i].access_time);
        }
    }
}
