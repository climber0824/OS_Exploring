#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    printf("Thread executing with argument: %d\n", *(int*)arg);
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    int arg = 42;

    // Initialize attributes
    pthread_attr_init(&attr);

    // Set stack size (1MB)
    pthread_attr_setstacksize(&attr, 1024 * 1024);

    // Set detach state
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Create thread with attributes
    if (pthread_create(&thread, &attr, thread_function, &arg) != 0) {
        perror("Thread creation failed");
        exit(1);
    }

    // Clean up attributes
    pthread_attr_destroy(&attr);

    // Wait for thread completion
    pthread_join(thread, NULL);

    return 0;
}

