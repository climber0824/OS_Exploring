#include <pthread.h>
#include <stdio.h>
#include <sched.h>

void* priority_thread(void* arg) {
    int policy;
    struct sched_param param;
    
    pthread_getschedparam(pthread_self(), &policy, &param);
    printf("Thread priority: %d\n", param.sched_priority);
    
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;
    
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_create(&thread, &attr, priority_thread, NULL);
    pthread_join(thread, NULL);
    
    pthread_attr_destroy(&attr);
    return 0;
}

