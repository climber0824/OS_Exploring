#include <stdlib.h>
#include <stdio.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

typedef struct {
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int request[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];
    int processes;
    int resources;    
} ResourceGraph;


// Initialize resource graph
void initializeRAG(ResourceGraph* graph, int p, int r) {
    graph->processes = p;
    graph->resources = r;

    // Initialize matrix
    for (int i = 0; i < p; ++i) {
        for (int j = 0; j < r; ++j) {
            graph->allocation[i][j] = 0;
            graph->request[i][j] = 0;
        }
    }

    // Initialize resources
    for (int i = 0; i < r; ++i) {
        graph->available[i] = 0;
    }
}


// Check for cycle in the graph (deadlock detection)
int detectDeadLock(ResourceGraph* graph) {
    int work[MAX_RESOURCES];
    int finish[MAX_RESOURCES] = {0};
    int deadlock = 0;

    // Initialize work array
    for (int i = 0; i < graph->resources; ++i) {
        work[i] = graph->available[i];
    }

    // Find an unfinished process that can be completed
    int found;
    do {
        found = 0;
        for (int i = 0; i < graph->processes; ++i) {
            int canComplete = 1;

            // Check if process can complete with available resoures
            for (int j = 0; j < graph->resources; ++j) {
                if (graph->request[i][j] > work[j]) {
                    canComplete = 0;
                    break;
                }
            }

            if (canComplete) {
                // Process can complete, release its resource
                for (int j = 0; j < graph->resources; ++j) {
                    work[j] += graph->allocation[i][j];
                }
                finish[i] = 1;
                found = 1;
            }
        }
    } while (found);

    // Check if all processes finished
    for (int i = 0; i < graph->processes; ++i) {
        if (!finish[i]) {
            deadlock = 1;
            break;
        }
    }

    return deadlock;
}


int main() {
    ResourceGraph graph;
    int p = 5, r = 3;

    initializeRAG(&graph, p, r);

    // Example resource allocation
    graph.allocation[0][0] = 1; graph.allocation[0][1] = 0; graph.allocation[0][2] = 0;
    graph.allocation[1][0] = 2; graph.allocation[1][1] = 0; graph.allocation[1][2] = 2;
    graph.allocation[2][0] = 3; graph.allocation[2][1] = 0; graph.allocation[2][2] = 2;
    graph.allocation[3][0] = 0; graph.allocation[3][1] = 1; graph.allocation[3][2] = 0;
    graph.allocation[4][0] = 0; graph.allocation[4][1] = 0; graph.allocation[4][2] = 2;
    
    // Example resource requests
    graph.request[0][0] = 0; graph.request[0][1] = 0; graph.request[0][2] = 0;
    graph.request[1][0] = 2; graph.request[1][1] = 0; graph.request[1][2] = 2;
    graph.request[2][0] = 0; graph.request[2][1] = 0; graph.request[2][2] = 0;
    graph.request[3][0] = 1; graph.request[3][1] = 0; graph.request[3][2] = 0;
    graph.request[4][0] = 0; graph.request[4][1] = 0; graph.request[4][2] = 2;
    
    // Available resources
    graph.available[0] = 3;
    graph.available[1] = 3;
    graph.available[2] = 2;
    
    if(detectDeadLock(&graph)) {
        printf("Deadlock detected!\n");
    } else {
        printf("No deadlock detected.\n");
    }

    return 0;
}
