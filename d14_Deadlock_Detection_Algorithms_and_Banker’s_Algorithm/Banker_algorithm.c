#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

typedef struct {
    int processes;
    int resources;

    // Main matrices
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int max[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];

    // For tracking safe sequence
    int safe_sequence[MAX_PROCESSES];
    bool finished[MAX_PROCESSES];
} BankerSystem;


// initialize
void initializeBankers(BankerSystem* bs, int p, int r) {
    bs->processes = p;
    bs->resources = r;

    for (int i = 0; i < p; ++i) {
        for (int j = 0; j < r; ++j) {
            bs->allocation[i][j] = 0;
            bs->allocation[i][j] = 0;
            bs->need[i][j] = 0;
        }
        bs->finished[i] = false;
    }

    for (int i = 0; i < r; ++i) {
        bs->available[i] = 0;
    }
}


void calNeed(BankerSystem* bs) {
    for (int i = 0; i < bs->processes; ++i) {
        for (int j = 0; j < bs->resources; ++j) {
            bs->need[i][j] = bs->max[i][j] - bs->allocation[i][j];
        }
    }
}



// check if resources can be allocated to a process
bool canAllocateResources(BankerSystem* bs, int process_id, int work[]) {
    for (int i = 0; i < bs->resources; ++i) {
        if (bs->need[process_id][i] > work[i]) {
            return false;
        }
    }
    return true;
}


// safety algorithm
bool isSafe(BankerSystem* bs) {
    int work[MAX_RESOURCES];
    int completed = 0;

    for (int i = 0; i < bs->resources; ++i) {
        work[i] = bs->available[i];
    }

    // reset
    for (int i = 0; i < bs->processes; ++i) {
        bs->finished[i] = false;        
    }

    // find a safe sequence
    while (completed < bs->processes) {
        bool found = false;

        for (int i = 0; i < bs->processes; ++i) {
            if (!bs->finished[i] && canAllocateResources(bs, i, work)) {
                // add process to safe sequence
                bs->safe_sequence[completed] = i;

                // add allocated resources back to work
                for (int j = 0; j < bs->resources; ++j) {
                    work[j] += bs->allocation[i][j];
                }

                bs->finished[i] = true;
                completed++;
                found = true;
            }
        }

        if (!found) {
            printf("System is not in safe state\n");
            return false;
        }
    }

    printf("System is in safe state.\nSafe sequence: ");
    for (int i = 0; i < bs->processes; ++i) {
        printf("%d, ", bs->safe_sequence[i]);
    }
    printf("\n");

    return true;
}


// Resource request algorithm
bool requestResources(BankerSystem* bs, int process_id, int request[]) {
    // Check if request is valid
    for (int i = 0; i < bs->resources; ++i) {
        if (request[i] > bs->need[process_id][i]) {
            printf("Error: process has exceeded its max\n");
            return false;
        }
        if (request[i] > bs->available[i]) {
            printf("Error: resoureces not avaible\n");
            return false;
        }
    }

    // Try to allocate resources
    for (int i = 0; i < bs->resources; ++i) {
        bs->available[i] -= request[i];
        bs->allocation[process_id][i] += request[i];
        bs->need[process_id][i] -= request[i];
    }

    // check if resulting state is safe
    if (isSafe(bs)) {
        printf("Resources allocated successfully to Process %d\n", process_id);
        return true;
    }
    
    // if not safe, rollback changes
    for (int i = 0; i < bs->resources; ++i) {
        bs->available[i] += request[i];
        bs->allocation[process_id][i] -= request[i];
        bs->need[process_id][i] += request[i];
    }

    printf("Request denied: Would lead to unsafe state!\n");
    return false;    
}



// Print current system state
void printSystemState(BankerSystem *bs) {
    printf("\nCurrent System State:\n");
    
    printf("\nAllocation Matrix:\n");
    for(int i = 0; i < bs->processes; i++) {
        for(int j = 0; j < bs->resources; j++) {
            printf("%d ", bs->allocation[i][j]);
        }
        printf("\n");
    }
    
    printf("\nMax Matrix:\n");
    for(int i = 0; i < bs->processes; i++) {
        for(int j = 0; j < bs->resources; j++) {
            printf("%d ", bs->max[i][j]);
        }
        printf("\n");
    }
    
    printf("\nNeed Matrix:\n");
    for(int i = 0; i < bs->processes; i++) {
        for(int j = 0; j < bs->resources; j++) {
            printf("%d ", bs->need[i][j]);
        }
        printf("\n");
    }
    
    printf("\nAvailable Resources:\n");
    for(int i = 0; i < bs->resources; i++) {
        printf("%d ", bs->available[i]);
    }
    printf("\n");
}


int main() {
    BankerSystem bs;
    int processes = 5;
    int resources = 3;
    
    initializeBankers(&bs, processes, resources);
    
    // Initialize Available Resources
    bs.available[0] = 3;
    bs.available[1] = 3;
    bs.available[2] = 2;
    
    // Initialize Allocation Matrix
    int allocation[5][3] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };
    
    // Initialize Maximum Matrix
    int max[5][3] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
    
    // Set up initial state
    for(int i = 0; i < processes; i++) {
        for(int j = 0; j < resources; j++) {
            bs.allocation[i][j] = allocation[i][j];
            bs.max[i][j] = max[i][j];
        }
    }
    
    calNeed(&bs);
    printSystemState(&bs);
    
    // Check if system is in safe state
    printf("\nChecking initial state safety...\n");
    isSafe(&bs);
    
    // Try to request resources
    printf("\nTesting resource request...\n");
    int request[3] = {1, 0, 2};
    requestResources(&bs, 1, request);
    
    return 0;
}

