#include <stdio.h>  
#include <stdlib.h>  
#include <pthread.h>  
#include <unistd.h>  
#include <sys/wait.h>  
#include <sys/time.h>  
#include <string.h>  
#include <sys/mman.h>  
#include <fcntl.h>  

#define NUM_ITEMS 1000  
#define SHARED_MEM_NAME "/shared_mem_example"  

// Requirements structure for decision making
typedef struct {
    int isolate_weight;
    int performance_weight;
    int communication_weight;
    int security_weight;
} requirements_t;

// Common data structure for both approaches
typedef struct {
    int* data;
    int start_index;
    int end_index;
} work_data_t;

// Common interface for both approaches
typedef struct {
    void (*initialize)(void* processor_data);
    void (*process_data)(void* processor_data, work_data_t* work);
    void (*clean_up)(void* processor_data);
    void* processor_data;
} concurrent_processor_t;

// Thread-based implementation
typedef struct {
    pthread_t* threads;
    int num_threads;
    int* shared_data;
    pthread_mutex_t mutex;
    work_data_t* work_units;
} thread_processor_t;

// Processor-based implementation
typedef struct {
    pid_t* processes;
    int num_processes;
    int* shared_memory;
    int shm_fd;
    work_data_t* work_units;
} process_processor_t;

// Decision making function
const char* suggest_implementation(requirement_t reqs) {
    int process_score = 0;  
    int thread_score = 0;  

    process_score += reqs.isolation_weight * 9;  
    process_score += reqs.performance_weight * 5;  
    process_score += reqs.communication_weight * 3;  
    process_score += reqs.security_weight * 9;  

    thread_score += reqs.isolation_weight * 3;  
    thread_score += reqs.performance_weight * 8;  
    thread_score += reqs.communication_weight * 9;  
    thread_score += reqs.security_weight * 4;  

    return (process_score > thread_score) ? "Use Processes" : "Use Threads";
}

// Thread worker function
void* thread_worker(void* arg) {
    work_data_t* work = (work_data_t*)arg;

    for (int i = work->start_index; i < work->end_index; ++i) {
        work->data[i] *= 2;     // simple data processing
    }

    return NULL;
}

// Thread implementaion function
void thread_initialize(void* processor_data) {
    thread_processor_t* tp = (thread_processor_t*)processor_data;
    tp->shared_data = malloc(NUM_ITEMS * sizeof(int));
    tp->work_units = malloc(tp->num_threads * sizeof(work_data_t));

    // Initialize data
    for (int i = 0; i < NUM_ITEMS; ++i) {
        tp->shared_data[i] = i;
    }

    pthread_mutex_init(&tp->mutex, NULL);
}


void thread_process_data(void* processor_data, work_data_t* work) {
    thread_processor_t* tp = (thread_processor_t*)processor_data;
    int items_per_thread = NUM_ITEMS / tp->num_threads;

    // create and start threads
    for (int i = 0; i < tp->num_threads; ++i) {
        tp->work_units[i].data = tp->shared_data;
        tp->work_units[i].start_index = i * items_per_thread;
        tp->work_units[i].end_index = (i == tp->num_threads - 1) ? 
                                        NUM_ITEMS : (i + 1) * items_per_thread;
        
        pthread_create(&tp->threads[i], NULL, thread_worker, &tp->work_units[i]);
    }
    
    
    // Wait for threads to complete
    for (int i = 0; i < tp->num_threads; ++i) {
        pthread_join(tp->threads[i], NULL);
    }
}


void thread_cleanup(void* processor_data) {
    thread_processor_t* tp = (thread_processor_t*) processor_data;
    pthread_mutex_destroy(&tp->mutex);
    free(tp->shared_data);
    free(tp->work_units);
    free(tp->threads);
    free(tp);
}





















