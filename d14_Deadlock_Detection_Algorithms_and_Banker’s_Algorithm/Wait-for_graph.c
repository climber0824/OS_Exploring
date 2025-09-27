#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10

typedef struct {
    int matrix[MAX_PROCESSES][MAX_PROCESSES];
    int processes;
} WaitForGraph;


// Initialize wait-for graph
void initializeWFG(WaitForGraph* graph, int p) {
    graph->processes = p;
    for (int i = 0; i < p; ++i) {
        for (int j = 0; j < p; ++j) {
            graph->matrix[i][j] = 0;
        }
    }    
}


// Detect cycle using DFS
int detectCycleDFS(WaitForGraph* graph, int vertex, int visited[], int recStack[]) {
    if (!visited[vertex]) {
        visited[vertex] = 1;
        recStack[vertex] = 1;

        for (int i = 0; i < graph->processes; ++i) {
            if (graph->matrix[vertex][i]) {
                if (!visited[i] && detectCycleDFS(graph, i, visited, recStack)) {
                    return 1;
                }
                else if (recStack[i]) {
                    return 1;
                }
            }
        }
    }
    recStack[vertex] = 0;
    
    return 0;
}


// detect deadlock in wait-for graph
int detectDeadLock(WaitForGraph* graph) {
    int visited[MAX_PROCESSES] = {0};
    int recStack[MAX_PROCESSES] = {0};

    for (int i = 0; i < graph->processes; ++i) {
        if (detectCycleDFS(graph, i, visited, recStack)) {
            return 1;
        }
    }

    return 0;
}


int main() {
    WaitForGraph graph;
    int p = 4;

    initializeWFG(&graph, 4);

    // Example: Process 0 waiting for Process 1
    graph.matrix[0][1] = 1;
    // Process 1 waiting for Process 2
    graph.matrix[1][2] = 1;
    // Process 2 waiting for Process 3
    graph.matrix[2][3] = 1;
    // Process 3 waiting for Process 0 (creates a cycle)
    graph.matrix[3][0] = 1;
    
    if(detectDeadLock(&graph)) {
        printf("Deadlock detected!\n");
    } else {
        printf("No deadlock detected.\n");
    }
    
    return 0;    
}


