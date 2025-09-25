#include <stdio.h>
#include <pthread.h>

int shared_counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int count = 1e6;

// Race condition
void* increment_without_mutex(void* arg) {
    for (int i = 0; i < count; ++i) {
        shared_counter++;
    }
    return NULL;
}


// Safe synchronized version
void* increment_with_mutex(void* arg) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < count; ++i) {
        shared_counter++;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}


int main() {
    int thread_nums = 2;
    pthread_t threads[thread_nums];

    // Demo race condition
    printf("Demonstrating race condition\n");
    shared_counter = 0;
    for (int i = 0; i < thread_nums; ++i) {
        pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
    }

    for (int i = 0; i < thread_nums; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter (unsafe): %d\n", shared_counter);

    // Demo safe condition
    printf("Demonstrating safe sync\n");
    shared_counter = 0;
    for (int i = 0; i < thread_nums; ++i) {
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    }

    for (int i = 0; i < thread_nums; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter (safe): %d\n", shared_counter);

    return 0;
}
