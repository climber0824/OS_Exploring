#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int shared_data = 0;
int data_ready = 0;

void* producer(void* arg) {
    pthread_mutex_lock(&mutex);
    shared_data = rand();
    data_ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (!data_ready) {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Received: %d\n", shared_data);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;
    int cnt = 10;

    for (int i = 0; i < cnt; ++i) {
        pthread_create(&cons_thread, NULL, consumer, NULL);
        pthread_create(&prod_thread, NULL, producer, NULL);

        pthread_join(prod_thread, NULL);
        pthread_join(cons_thread, NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}
