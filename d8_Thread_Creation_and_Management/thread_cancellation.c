#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* cancellable_thread(void* arg) {
    int i = 0;
    while (1) {
        printf("Thread iteration %d\n", ++i);
        sleep(1);
        pthread_testcancel();  // Cancellation point
    }
    return NULL;
}

int main() {
    pthread_t thread;
    
    pthread_create(&thread, NULL, cancellable_thread, NULL);
    sleep(3);  // Let thread run for 3 seconds
    
    printf("Cancelling thread...\n");
    pthread_cancel(thread);
    
    pthread_join(thread, NULL);
    printf("Thread cancelled and joined\n");

    return 0;
}

