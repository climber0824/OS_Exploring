#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* return_value_thread(void* arg) {
    int* result = malloc(sizeof(int));
    *result = 42;
    return (void*)result;
}

int main() {
    pthread_t thread;
    void* result;

    pthread_create(&thread, NULL, return_value_thread, NULL);
    
    // Wait for thread and get return value
    pthread_join(thread, &result);
    
    printf("Thread returned: %d\n", *(int*)result);
    free(result);

    return 0;
}

