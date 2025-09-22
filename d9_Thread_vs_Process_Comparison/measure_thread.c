#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAY_SIZE 1e4
#define NUM_THREADS 4

double* shared_array;

void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    int chunk_size = ARRAY_SIZE / NUM_THREADS;
    int start = thread_id * chunk_size;
    int end = start + chunk_size;
    
    for (int i = start; i < end; i++) {
        shared_array[i] = shared_array[i] * 2.0;
    }
    
    return NULL;
}

int main() {
    shared_array = malloc(ARRAY_SIZE * sizeof(double));
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_array[i] = (double)i;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }
    
    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    
    long microseconds = (end.tv_sec - start.tv_sec) * 1000000 +
                       (end.tv_usec - start.tv_usec);
    printf("Thread processing time: %ld microseconds\n", microseconds);
    
    free(shared_array);
    return 0;
}
