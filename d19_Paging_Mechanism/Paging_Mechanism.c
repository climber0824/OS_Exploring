#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
    int* frames;
    int capacity;
    int size;
    int head;
} FIFOQueue;


typedef struct {
    int page;
    int last_used;    
} PageFrame;


FIFOQueue* initFIFOQueue(int capacity) {
    FIFOQueue* queue = (FIFOQueue*)malloc(sizeof(FIFOQueue));
    queue->frames = (int*)malloc(capacity * sizeof(int));
    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;

    return queue;
}


bool isPagePresent(FIFOQueue* queue, int page) {
    for (int i = 0; i < queue->size; ++i) {
        if (queue->frames[i] == page) {
            return true;
        }
    }

    return false;
}


void FIFOPageReplacement(int pages[], int n, int capacity) {
    FIFOQueue* queue = initFIFOQueue(capacity);
    int page_faults = 0;

    for (int i = 0; i < n; ++i) {
        if (!isPagePresent(queue, pages[i])) {
            page_faults++;

            if (queue->size < queue->capacity) {
                queue->frames[queue->size++] = pages[i];
            }
            else {
                queue->frames[queue->head] = pages[i];
                queue->head = (queue->head + 1) % queue->capacity;
            }

            printf("Page fault occured for page %d \n", pages[i]);
        }
    }

    printf("Total page faults: %d\n", page_faults);
    free(queue->frames);
    free(queue);
}


void LRUPageReplacement(int pages[], int n, int capacity) {
    PageFrame* frames = (PageFrame*)malloc(sizeof(PageFrame) * capacity);
    int page_faults = 0;
    int curr_size = 0;

    for (int i = 0; i < capacity; ++i) {
        frames[i].page = -1;
        frames[i].last_used = -1;
    }

    for (int i = 0; i < n; ++i) {
        int page = pages[i];
        bool page_found = false;

        // check if page already exists
        for (int j = 0; j < curr_size; ++j) {
            if (frames[j].page == page) {
                frames[j].last_used = i;
                page_found = true;
                break;
            }
        }

        if (!page_found) {
            page_faults++;

            if (curr_size < capacity) {
                frames[curr_size].page = page;
                frames[curr_size].last_used = i;
                curr_size++;
            }
            else {
                // find least recently used page
                int lru_idx = 0;
                int min_last_used = INT_MAX;

                for (int j = 0; j < capacity; ++j) {
                    if (frames[j].last_used < min_last_used) {
                        min_last_used = frames[j].last_used;
                        lru_idx = j;
                    }
                }

                frames[lru_idx].page = page;
                frames[lru_idx].last_used = i;
            }

            printf("page fault occured for page %d\n", page);
        }
    }

    printf("Total page faults: %d\n", page_faults);
    free(frames);
}


int findFarthest(int pages[], int n, int curr_idx, int* frames, int capacity) {
    int farthest = -1;
    int farthest_idx = -1;

    for (int i = 0; i < capacity; ++i) {
        int j;
        for (j = curr_idx; j < n; ++j) {
            if (frames[i] == pages[j]) {
                if (j >farthest) {
                    farthest = j;
                    farthest_idx = i;
                }
                break;
            }
        }
        if (j == n) return i;   // page not referenced in future
    }

    return (farthest_idx == -1) ? 0 : farthest_idx;
}


void optimalPageReplacement(int pages[], int n, int capacity) {
    int* frames = (int*)malloc(sizeof(int) * capacity);
    int page_faults = 0, curr_size = 0;

    for (int i = 0; i < capacity; ++i) {
        frames[i] = -1;
    }

    for (int i = 0; i < n; ++i) {
        bool page_found = false;

        for (int j = 0; j < curr_size; ++j) {
            if (frames[j] == pages[i]) {
                page_found = true;
                break;
            }
        }

        if (!page_found) {
            page_faults++;

            if (curr_size < capacity) {
                frames[curr_size++] = pages[i];
            }
            else {
                int replace_idx = findFarthest(pages, n, i+1, frames, capacity);
                frames[replace_idx] = pages[i];
            }

            printf("Page fault occured for page %d\n", pages[i]);
        }
    }

    printf("Total page faults: %d\n", page_faults);
    free(frames);
}


int main() {
    return 0;
}
