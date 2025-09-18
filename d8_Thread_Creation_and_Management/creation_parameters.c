#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int thread_id;
    char* message;
    int sleep_time;
} thread_params_t;

void* parameterized_thread(void* arg) {
    thread_params_t* params = (thread_params_t*)arg;
    
    printf("Thread %d starting with message: %s\n", 
           params->thread_id, params->message);
    
    sleep(params->sleep_time);
    
    printf("Thread %d finishing\n", params->thread_id);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[3];
    thread_params_t params[3] = {
        {1, "First Thread", 2},
        {2, "Second Thread", 3},
        {3, "Third Thread", 1}
    };

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, parameterized_thread, &params[i]) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
