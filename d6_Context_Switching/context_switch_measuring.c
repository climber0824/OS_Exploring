// Process Affinity
#define _GUN_SOURCE #include void set_cpu_addinity(int cpu_id) { cpu_set_t cpuset; \
CPU_ZERO(&cpuset); \
CPU_SET(cpu_id, &cpu_set); \
sched_setaffinity(0, sizeof(cpu_set), &cpu_set); }

// TLB Optimization
// Example of TLB optimization code
static inline void flush_tlb_single(unsigned long addr) {
    asm volatile ("invlpg (%0)" :: "r" (addr) : "memory");
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_SWITCHES 1000

typedef struct {
    struct timespec start_time;
    struct timespec end_time;
    long long total_time;
} timing_info_t;

void measure_context_switch_overhead(timing_info_t *timing) {
    pid_t pid;
    int pipe_fd[2];
    char buf[1];

    pipe(pipe_fd);

    clock_gettime(CLOCK_MONOTONIC, &timing->start_time);

    pid = fork();
    if (pid == 0) {     // child process
        for (int i = 0; i < NUM_SWITCHES; ++i) {
            read(pipe_fd[0], buf, 1);
            write(pipe_fd[1], "x", 1);
        }
        exit(0);
    }
    else {              // parent process
        for (int i = 0; i < NUM_SWITCHES; ++i) {
            write(pipe_fd[1], "x", 1);
            read(pipe_fd[0], buf, 1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &timing->end_time);

    timing->total_time = (timing->end_time.tv_sec - timing->start_time.tv_sec) * 1000000000LL + (timing->end_time.tv_nsec - timing->start_time.tv_nsec);

}

int main() {
    timing_info_t timing;

    printf("Measuring context switch overhead...\n");
    measure_context_switch_overhead(&timing);

    printf("Average context switch time: %lld ns \n",
            timing.total_time / (NUM_SWITCHES * 2));

    return 0;
}



















