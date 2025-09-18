#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_key_t thread_key;

void cleanup_thread_data(void* data) {
    free(data);
}

void* thread_function(void* arg) {
    int* data = malloc(sizeof(int));
    *data = *(int*)arg;
    
    pthread_setspecific(thread_key, data);
    
    // Access thread-local storage
    int* tls_data = pthread_getspecific(thread_key);
    printf("Thread %d: TLS value = %d\n", *((int*)arg), *tls_data);
    
    return NULL;
}

int main() {
    pthread_t threads[3];
    int thread_args[3] = {1, 2, 3};
    
    pthread_key_create(&thread_key, cleanup_thread_data);
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_key_delete(thread_key);
    return 0;
}


