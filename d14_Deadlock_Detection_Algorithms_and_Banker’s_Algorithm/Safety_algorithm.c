#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10


typedef struct  {
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int max[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];
    int processes;
    int resources;    
} BankerState;


// Initialize Banker's algo state
void initializeBankers(BankerState* state, int p, int r) {
    state->processes = p;
    state->resources = r;

    for (int i = 0; i < p; ++i) {
        for (int j = 0; j < r; ++j) {
            state->allocation[i][j] = 0;
            state->max[i][j] = 0;
            state->need[i][j] = 0;
        }        
    }

    for (int i = 0; i < r; ++i) {
        state->available[i] = 0;
    }
}


// calculate need matrix
void calculateNeed(BankerState* state) {
    for (int i = 0; i < state->processes; ++i) {
        for (int j = 0; j < state->resources; ++j) {
            state->need[i][j] = state->max[i][j] - state->allocation[i][j];
        }
    }
}


// check if system is in safe state
int isSafe(BankerState* state) {
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int safeSequence[MAX_PROCESSES];
    int count = 0;

    for (int i = 0; i < state->resources; ++i) {
        work[i] = state->available[i];
    }

    while (count < state->processes) {
        int found = 0;

        for (int i = 0; i < state->processes; ++i) {
            if (!finish[i]) {
                int canAllocate = 1;

                for (int j = 0; j < state->resources; ++j) {
                    if (state->need[i][j] > work[j]) {
                        canAllocate = 0;
                        break;
                    }
                }

                if (canAllocate) {
                    for (int j = 0; j < state->resources; ++j) {
                        work[j] += state->allocation[i][j];
                    }

                    safeSequence[count] = i;
                    finish[i] = 1;
                    count++;
                    found = 1;
                }
            }
        }
        if (!found) {
            printf("System is not in safe state\n");
            return 0;
        }
    }

    printf("System is in safe state.\nSafe sequence: ");
    for (int i = 0; i < state->processes; ++i) {
        printf("%d", safeSequence[i]);
    }
    printf("\n");

    return 1;
}


int main() {
    BankerState state;
    int p = 5, r = 3;
    
    initializeBankers(&state, p, r);
    
    // Example allocation matrix
    int allocation[5][3] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };
    
    // Example max matrix
    int max[5][3] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
    
    // Available resources
    state.available[0] = 3;
    state.available[1] = 3;
    state.available[2] = 2;
    
    // Copy matrices
    for(int i = 0; i < p; i++) {
        for(int j = 0; j < r; j++) {
            state.allocation[i][j] = allocation[i][j];
            state.max[i][j] = max[i][j];
        }
    }
    
    calculateNeed(&state);
    isSafe(&state);
    
    return 0;
}

