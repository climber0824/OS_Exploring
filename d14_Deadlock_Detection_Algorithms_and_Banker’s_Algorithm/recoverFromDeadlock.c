#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

typedef struct {
    int pid;
    int priority;
    int age;
    int resources_held[MAX_RESOURCES];
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int num_processes;
    int num_resources;
} DeadlockRecovery;


// Initialize recovery system
void initializeRecovery(DeadlockRecovery* dr, int num_proc, int num_res) {
    dr->num_processes = num_proc;
    dr->num_resources = num_res;

    for (int i = 0; i < num_proc; ++i) {
        dr->processes[i].pid = i;
        dr->processes[i].priority = rand() % 10;    // 0-9
        dr->processes[i].age = 0;

        for (int j = 0; j < num_res; ++j) {
            dr->processes[i].resources_held[j] = 0; 
        }
    }
}


// select victim process for termination
int selectVictimProcess(DeadlockRecovery* dr) {
    int victim = -1;
    int lowest_priority = 999;

    for (int i = 0; i < dr->num_processes; ++i) {
        int curr_priority = dr->processes[i].priority - (dr->processes[i].age / 10);

        if (curr_priority < lowest_priority) {
            lowest_priority = curr_priority;
            victim = i;
        }
    }

    return victim;
}


// release sources held bu terminated process
void releaseResources(DeadlockRecovery* dr, int victim) {
    printf("Releasing resources from %d\n", victim);

    for (int i = 0; i < dr->num_resources; ++i) {
        if (dr->processes[victim].resources_held[i] > 0) {
            printf("Release resource %d\n", i);
            dr->processes[victim].resources_held[i] = 0;
        }
    }
} 


// handle deadlock recovery
void recoverFromDeadlock(DeadlockRecovery* dr) {
    int victim = selectVictimProcess(dr);

    if (victim != -1) {
        printf("Select process %d for termination.\n", victim);
        releaseResources(dr, victim);

        // age remaining processes
        for (int i = 0; i < dr->num_processes; ++i) {
            if (i != victim) {
                dr->processes[i].age++;
            }
        }
    }
}


int main() {
    DeadlockRecovery dr;
    initializeRecovery(&dr, 5, 3);
    
    // Simulate resource allocation
    dr.processes[0].resources_held[0] = 1;
    dr.processes[1].resources_held[1] = 1;
    dr.processes[2].resources_held[2] = 1;
    
    // Simulate deadlock detection and recovery
    printf("Simulating deadlock recovery...\n");
    recoverFromDeadlock(&dr);
    
    return 0;
}
