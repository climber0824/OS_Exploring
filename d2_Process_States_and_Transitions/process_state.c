#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Process state enumeration
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    ProcessState state;
    int priority;
} PCB;

void transition_state(PCB *process, ProcessState new_state) {
    printf("transition process %d: %d -> %d\n",
            process->pid, process->state, new_state);
    process->state = new_state;
}

int main() {
    PCB process;
    process.pid = getpid();
    process.state = NEW;
    process.priority = 5;

    // Simulating state transitions
    transition_state(&process, READY);
    sleep(1);  // Simulate preparation time
    
    transition_state(&process, RUNNING);
    sleep(2);  // Simulate execution
    
    transition_state(&process, WAITING);
    sleep(1);  // Simulate waiting for resource
    
    transition_state(&process, READY);
    sleep(1);
    
    transition_state(&process, TERMINATED);

    PCB* processPtr;
    processPtr = (PCB*) malloc(sizeof(PCB));

    if (processPtr == NULL) {
        perror("fail to allocate\n");
        return 1;
    }

    processPtr->pid = getpid();
    processPtr->state = NEW;
    processPtr->priority = 5;
    
    transition_state(processPtr, READY);
    sleep(1);

    transition_state(processPtr, RUNNING);
    sleep(2);

    transition_state(processPtr, READY);
    sleep(1);

    transition_state(processPtr, TERMINATED);
    free(processPtr);

    return 0;
}
