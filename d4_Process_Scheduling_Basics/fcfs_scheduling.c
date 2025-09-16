#include <stdio.h>

#define MAX_PROCESSES 10

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
} Process;

void fcfs_scheduling(Process processes[], int n) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    processes[0].waiting_time = 0;
    processes[0].turnaround_time = processes[0].burst_time;

    for (int i = 1; i < n; i++) {
        processes[i].waiting_time = 
            processes[i-1].waiting_time + processes[i-1].burst_time;
        
        processes[i].turnaround_time = 
            processes[i].waiting_time + processes[i].burst_time;
    }

    printf("Process\tArrival\tBurst\tWaiting\tTurnaround\n");
    
    for (int i = 0; i < n; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;

        printf("%d\t%d\t%d\t%d\t%d\n", 
               processes[i].process_id, 
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time);
    }

    printf("Average Waiting Time: %.2f\n", 
           (float)total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", 
           (float)total_turnaround_time / n);
}

int main() {
    Process processes[MAX_PROCESSES] = {
        {1, 0, 10},
        {2, 1, 5},
        {3, 3, 8}
    };

    fcfs_scheduling(processes, 3);
    return 0;
}
