#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 3
pthread_barrier_t barrier;

void* barrier_thread(void* arg) {
    int id = *(int*)arg;
    
    printf("Thread %d before barrier\n", id);
    pthread_barrier_wait(&barrier);
    printf("Thread %d after barrier\n", id);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, barrier_thread, &thread_ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    return 0;
}

