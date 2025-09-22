#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>

// Timing structure
typedef struct {
    struct timeval start;
    struct timeval end;
} timing_t;

// Function to measure process creation time
long measure_process_creation() {
    timing_t timing;
    gettimeofday(&timing.start, NULL);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit(0);
    } else {
        // Parent process
        wait(NULL);
    }
    
    gettimeofday(&timing.end, NULL);
    
    long microseconds = (timing.end.tv_sec - timing.start.tv_sec) * 1000000 +
                       (timing.end.tv_usec - timing.start.tv_usec);
    printf("Process creation time: %ld microseconds\n", microseconds);
    
    return microseconds;
}


// Function for thread creation measurement
void* thread_function(void* arg) {
    return NULL;
}

long measure_thread_creation() {
    timing_t timing;
    pthread_t thread;
    
    gettimeofday(&timing.start, NULL);
    
    pthread_create(&thread, NULL, thread_function, NULL);
    pthread_join(thread, NULL);
    
    gettimeofday(&timing.end, NULL);
    
    long microseconds = (timing.end.tv_sec - timing.start.tv_sec) * 1000000 +
                       (timing.end.tv_usec - timing.start.tv_usec);
    printf("Thread creation time: %ld microseconds\n", microseconds);
    
    return microseconds;
}


int main() {
    int count = 10; 
    long thread_create_avg_time = 0.0, process_create_avg_time = 0.0;

    for (int i = 0; i < count; ++i) {
        process_create_avg_time += measure_process_creation();
        thread_create_avg_time += measure_thread_creation();
    }

    process_create_avg_time /= count;
    thread_create_avg_time /= count;

    printf("Process create average time: %ld\n", process_create_avg_time);
    printf("Thread create average time: %ld\n", thread_create_avg_time);
    

    return 0;
}


