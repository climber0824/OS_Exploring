#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    printf("Producer: Data is ready\n");
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {
        printf("Consumer: Waiting for data\n");
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Consumer: Got data\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;
    
    pthread_create(&cons_thread, NULL, consumer, NULL);
    sleep(1);  // Ensure consumer starts first
    pthread_create(&prod_thread, NULL, producer, NULL);
    
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}

