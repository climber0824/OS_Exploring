#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t data_cond = PTHREAD_COND_INITIALIZER;
int data_ready = 0;
double shared_data = 0.0;

void* real_time_producer(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        shared_data = i * 1.5;
        data_ready = 1;
        pthread_cond_signal(&data_cond);
        pthread_mutex_unlock(&mutex);
        usleep(100000);  // 100ms
    }
    return NULL;
}

void* real_time_consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (!data_ready) {
            pthread_cond_wait(&data_cond, &mutex);
        }
        printf("Received data: %f\n", shared_data);
        data_ready = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

