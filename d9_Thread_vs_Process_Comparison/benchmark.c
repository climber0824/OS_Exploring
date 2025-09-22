#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define NUM_ITERATIONS 1e5
#define NUM_RUNS 10

typedef struct {
    long thread_times[NUM_RUNS];
    long process_times[NUM_RUNS];
} benchmark_results_t;

void* thread_work(void* arg) {
    // Simulate some work
    double result = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        result += i * 1.5;
    }
    return NULL;
}

void process_work() {
    double result = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        result += i * 1.5;
    }
}

benchmark_results_t run_benchmarks() {
    benchmark_results_t results;
    struct timeval start, end;
    
    for (int run = 0; run < NUM_RUNS; run++) {
        // Thread benchmark
        gettimeofday(&start, NULL);
        
        pthread_t thread;
        pthread_create(&thread, NULL, thread_work, NULL);
        pthread_join(thread, NULL);
        
        gettimeofday(&end, NULL);
        results.thread_times[run] = (end.tv_sec - start.tv_sec) * 1000000 +
                                  (end.tv_usec - start.tv_usec);
        
        // Process benchmark
        gettimeofday(&start, NULL);
        
        pid_t pid = fork();
        if (pid == 0) {
            process_work();
            exit(0);
        } else {
            wait(NULL);
        }
        
        gettimeofday(&end, NULL);
        results.process_times[run] = (end.tv_sec - start.tv_sec) * 1000000 +
                                   (end.tv_usec - start.tv_usec);
    }
    
    return results;
}

void analyze_results(benchmark_results_t results) {
    double thread_avg = 0.0, process_avg = 0.0;
    long thread_min = results.thread_times[0];
    long thread_max = results.thread_times[0];
    long process_min = results.process_times[0];
    long process_max = results.process_times[0];
    
    for (int i = 0; i < NUM_RUNS; i++) {
        thread_avg += results.thread_times[i];
        process_avg += results.process_times[i];
        
        if (results.thread_times[i] < thread_min)
            thread_min = results.thread_times[i];
        if (results.thread_times[i] > thread_max)
            thread_max = results.thread_times[i];
        if (results.process_times[i] < process_min)
            process_min = results.process_times[i];
        if (results.process_times[i] > process_max)
            process_max = results.process_times[i];
    }
    
    thread_avg /= NUM_RUNS;
    process_avg /= NUM_RUNS;
    
    printf("Thread Performance:\n");
    printf("  Average: %.2f microseconds\n", thread_avg);
    printf("  Min: %ld microseconds\n", thread_min);
    printf("  Max: %ld microseconds\n", thread_max);
    printf("\nProcess Performance:\n");
    printf("  Average: %.2f microseconds\n", process_avg);
    printf("  Min: %ld microseconds\n", process_min);
    printf("  Max: %ld microseconds\n", process_max);
}

int main() {
    printf("Running benchmarks...\n");
    benchmark_results_t results = run_benchmarks();
    printf("\nBenchmark Results:\n");
    analyze_results(results);
    return 0;
}
