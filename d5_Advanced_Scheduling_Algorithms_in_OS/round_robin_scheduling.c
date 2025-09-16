#include <stdio.h>

#define MAX_PROCESSES 100
#define TIME_QUANTUM 2

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
} Process;

void round_robin_scheduling(Process processes[], int n) {
    int current_time = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;
    int completed = 0;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0) {
                // Execute process for time quantum or remaining time
                int execute_time = (processes[i].remaining_time < TIME_QUANTUM) 
                    ? processes[i].remaining_time 
                    : TIME_QUANTUM;

                processes[i].remaining_time -= execute_time;
                current_time += execute_time;

                // If process completed
                if (processes[i].remaining_time == 0) {
                    completed++;
                    processes[i].turnaround_time = current_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

                    total_waiting_time += processes[i].waiting_time;
                    total_turnaround_time += processes[i].turnaround_time;
                }
            }
        }
    }

    // Print results
    printf("Round Robin Scheduling Results:\n");
    printf("Process\tArrival\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", 
               processes[i].process_id, 
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

int main() {
    Process processes[] = {
        {1, 0, 10, 10},   // Process ID, Arrival Time, Burst Time, Remaining Time
        {2, 1, 5, 5},
        {3, 3, 8, 8}
    };
    int n = sizeof(processes) / sizeof(processes[0]);

    round_robin_scheduling(processes, n);
    return 0;
}

