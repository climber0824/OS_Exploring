#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Shared data structure
typedef struct {
    int process_value;
    int thread_value;
} shared_data_t;

// Global variables
shared_data_t* shared_memory;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_function(void* arg) {
    // Thread can access process memory directly
    shared_memory->thread_value++;
    return NULL;
}

int main() {
    // Create shared memory for processes
    shared_memory = mmap(NULL, sizeof(shared_data_t),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    shared_memory->process_value = 0;
    shared_memory->thread_value = 0;
    
    // Create process
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        shared_memory->process_value++;
        exit(0);
    } else {
        // Parent process
        pthread_t thread;
        pthread_create(&thread, NULL, thread_function, NULL);
        pthread_join(thread, NULL);
        
        wait(NULL);
        
        printf("Process value: %d\n", shared_memory->process_value);
        printf("Thread value: %d\n", shared_memory->thread_value);
        
        munmap(shared_memory, sizeof(shared_data_t));
    }
    
    return 0;
}

