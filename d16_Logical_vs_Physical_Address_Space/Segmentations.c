/*
Segmentation is a memory management technique that divides a program’s address space 
into logical segments, each representing a different part of the program, 
like code, data, or stack. Each segment has a base address and a limit, 
defining its starting location and size in memory. 

Todo:
1. Use appropriate page sizes for different memory regions.
2. Implement efficient TLB management.
3. Monitor and optimize page fault rates.
4. Implement proper memory protection mechanisms.
5. Use memory alignment for better performance.
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_SEGMENTS 10

typedef struct {
    unsigned int base;
    unsigned int limit;
    unsigned int protection;
} SegmentDescriptor;

typedef struct {
    SegmentDescriptor* segments;
    int num_segments;
} SegmentTable;


SegmentTable* initSegmentTable() {
    SegmentTable* st = (SegmentTable*)malloc(sizeof(SegmentTable));
    st->segments = (SegmentDescriptor*)calloc(MAX_SEGMENTS, sizeof(SegmentDescriptor));
    st->num_segments = 0;

    return st;
}


int addSegment(SegmentTable* st, unsigned int base, unsigned int limit, unsigned int protection) {
    if (st->num_segments >= MAX_SEGMENTS) {
        return -1;
    }

    st->segments[st->num_segments].base = base;
    st->segments[st->num_segments].limit = limit;
    st->segments[st->num_segments].protection = protection;

    return st->num_segments - 1;
}
