#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t lock;
int shared_counter = 0;

void* thread_function(void* arg) {
    int thread_id = *(int* )arg;
    printf("Thread %d is running\n", thread_id);
    return NULL;
}

void* increment_counter(void* arg) {
    int count = 100000;
    for (int i = 0; i < count; ++i) {
        pthread_mutex_lock(&lock);
        shared_counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    int thread_nums = 5;
    pthread_t threads[thread_nums];
    int thread_ids[thread_nums];

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < thread_nums; ++i) {
        if (pthread_create(&threads[i], NULL, increment_counter, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    for (int i = 0; i < thread_nums; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("Final counter value: %d\n", shared_counter);

    printf("All threads have finished execution\n");
    return 0;
}
