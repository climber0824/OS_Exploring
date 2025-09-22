#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_ITERATIONS 1e4

void measure_context_switch_overhead() {
    int pipe_fd[2];
    pipe(pipe_fd);
    
    struct timeval start, end;
    char buffer[1];
    
    gettimeofday(&start, NULL);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            read(pipe_fd[0], buffer, 1);
            write(pipe_fd[1], "x", 1);
        }
        exit(0);
    } else {
        // Parent process
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            write(pipe_fd[1], "x", 1);
            read(pipe_fd[0], buffer, 1);
        }
    }
    
    gettimeofday(&end, NULL);
    
    long microseconds = (end.tv_sec - start.tv_sec) * 1000000 +
                       (end.tv_usec - start.tv_usec);
    printf("Average context switch time: %f microseconds\n",
           (float)microseconds / (NUM_ITERATIONS * 2));
}

int main() {
    measure_context_switch_overhead();
    return 0;
}


