#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* detached_thread(void* arg) {
    printf("Detached thread running\n");
    sleep(2);
    printf("Detached thread finishing\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&thread, &attr, detached_thread, NULL);
    pthread_attr_destroy(&attr);

    printf("Main thread continuing...\n");
    sleep(3);  // Wait to see detached thread output
    printf("Main thread exiting\n");

    return 0;
}

