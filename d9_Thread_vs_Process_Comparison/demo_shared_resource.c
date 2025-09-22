#include <pthread.h>
#include <stdio.h>

typedef struct {
    int* shared_counter;
    pthread_mutex_t* mutex;
} shared_resource_t;

void* increment_counter(void* arg) {
    shared_resource_t* resource = (shared_resource_t*)arg;
    
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(resource->mutex);
        (*resource->shared_counter)++;
        pthread_mutex_unlock(resource->mutex);
    }
    
    return NULL;
}

void demonstrate_shared_resource() {
    int counter = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    shared_resource_t resource = {&counter, &mutex};
    
    pthread_t threads[4];
    
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, increment_counter, &resource);
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Final counter value: %d\n", counter);
    pthread_mutex_destroy(&mutex);
}

