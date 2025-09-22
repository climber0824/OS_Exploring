#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#define MATRIX_SIZE 500
#define NUM_WORKERS 4

typedef struct {
    double* a;
    double* b;
    double* c;
    int start_row;
    int end_row;
} matrix_work_t;

void* thread_matrix_multiply(void* arg) {
    matrix_work_t* work = (matrix_work_t*)arg;
    
    for (int i = work->start_row; i < work->end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += work->a[i * MATRIX_SIZE + k] * 
                       work->b[k * MATRIX_SIZE + j];
            }
            work->c[i * MATRIX_SIZE + j] = sum;
        }
    }
    
    return NULL;
}

void process_matrix_multiply(double* a, double* b, double* c,
                           int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * 
                       b[k * MATRIX_SIZE + j];
            }
            c[i * MATRIX_SIZE + j] = sum;
        }
    }
}

void compare_performance() {
    // Allocate matrices
    size_t matrix_bytes = MATRIX_SIZE * MATRIX_SIZE * sizeof(double);
    
    double* a = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double* b = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double* c = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Initialize matrices
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
    }
    
    struct timeval start, end;
    
    // Thread implementation
    gettimeofday(&start, NULL);
    
    pthread_t threads[NUM_WORKERS];
    matrix_work_t thread_work[NUM_WORKERS];
    int rows_per_worker = MATRIX_SIZE / NUM_WORKERS;
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        thread_work[i].a = a;
        thread_work[i].b = b;
        thread_work[i].c = c;
        thread_work[i].start_row = i * rows_per_worker;
        thread_work[i].end_row = (i == NUM_WORKERS - 1) ? 
                                MATRIX_SIZE : (i + 1) * rows_per_worker;
        
        pthread_create(&threads[i], NULL,
                      thread_matrix_multiply, &thread_work[i]);
    }
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    long thread_time = (end.tv_sec - start.tv_sec) * 1000000 +
                      (end.tv_usec - start.tv_usec);
    
    // Process implementation
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            int start_row = i * rows_per_worker;
            int end_row = (i == NUM_WORKERS - 1) ?
                         MATRIX_SIZE : (i + 1) * rows_per_worker;
            
            process_matrix_multiply(a, b, c, start_row, end_row);
            exit(0);
        }
    }
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        wait(NULL);
    }
    
    gettimeofday(&end, NULL);
    long process_time = (end.tv_sec - start.tv_sec) * 1000000 +
                       (end.tv_usec - start.tv_usec);
    
    printf("Thread implementation time: %ld microseconds\n", thread_time);
    printf("Process implementation time: %ld microseconds\n", process_time);
    
    // Clean up
    munmap(a, matrix_bytes);
    munmap(b, matrix_bytes);
    munmap(c, matrix_bytes);
}

int main() {
    compare_performance();
    return 0;
}

