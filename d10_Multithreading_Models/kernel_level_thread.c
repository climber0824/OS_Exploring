#include <pthread.h>
#include <stdio.h>


void* kernel_thread_func(void* arg) {
    printf("Kernel thread function\n");
    
    return NULL;
}


int main() {
    pthread_t kthread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&kthread, &attr, kernel_thread_func, NULL);
    pthread_join(kthread, NULL);

    return 0;
}




