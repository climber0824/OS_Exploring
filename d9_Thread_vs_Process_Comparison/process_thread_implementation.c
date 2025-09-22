#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>

#define ARRAY_SIZE 1e5
#define NUM_WORKERS 4

// Shared data structure
typedef struct {
    double* array;
    int start;
    int end;
} work_unit_t;

// Thread worker function
void* thread_worker(void* arg) {
    work_unit_t* unit = (work_unit_t*)arg;
    
    for (int i = unit->start; i < unit->end; i++) {
        unit->array[i] = unit->array[i] * 2.0;
    }
    
    return NULL;
}

// Process worker function
void process_worker(double* array, int start, int end) {
    for (int i = start; i < end; i++) {
        array[i] = array[i] * 2.0;
    }
}

// Thread implementation
void thread_implementation() {
    double* array = malloc(ARRAY_SIZE * sizeof(double));
    pthread_t threads[NUM_WORKERS];
    work_unit_t units[NUM_WORKERS];
    
    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (double)i;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Create threads
    int chunk_size = ARRAY_SIZE / NUM_WORKERS;
    for (int i = 0; i < NUM_WORKERS; i++) {
        units[i].array = array;
        units[i].start = i * chunk_size;
        units[i].end = (i == NUM_WORKERS - 1) ? ARRAY_SIZE : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, thread_worker, &units[i]);
    }
    
    // Wait for threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    
    long microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Thread implementation time: %ld microseconds\n", microseconds);
    
    free(array);
}

// Process implementation
void process_implementation() {
    double* array = mmap(NULL, ARRAY_SIZE * sizeof(double),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (double)i;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Create processes
    int chunk_size = ARRAY_SIZE / NUM_WORKERS;
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            int start = i * chunk_size;
            int end = (i == NUM_WORKERS - 1) ? ARRAY_SIZE : (i + 1) * chunk_size;
            process_worker(array, start, end);
            exit(0);
        }
    }
    
    // Wait for all child processes
    for (int i = 0; i < NUM_WORKERS; i++) {
        wait(NULL);
    }
    
    gettimeofday(&end, NULL);
    
    long microseconds = (end.tv_sec - start.tv_sec) * 1000000 +
                       (end.tv_usec - start.tv_usec);
    printf("Process implementation time: %ld microseconds\n", microseconds);
    
    munmap(array, ARRAY_SIZE * sizeof(double));
}

int main() {
    printf("Running thread implementation...\n");
    thread_implementation();
    
    printf("\nRunning process implementation...\n");
    process_implementation();
    
    return 0;
}
