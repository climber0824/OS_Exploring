/*
Todo:
1. Implement thread pools to reduce thread creation overhead and manage system resources effectively. Thread pools maintain a set of pre-created threads that can be reused for different tasks.

2. Choose the appropriate number of threads based on the system’s hardware capabilities and workload characteristics. Too many threads can lead to context switching overhead.

3. Implement proper cleanup mechanisms for thread resources to prevent memory leaks and resource exhaustion.

*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4
#define MAX_QUEUE_SIZE 100

typedef struct {
    void (*function)(void*);
    void* arg;
} task_t;

typedef struct {
    task_t* task_queue;
    int queue_size;
    int front;
    int rear;
    pthread_mutex_t queue_mutex;
    pthread_cond_t  queue_not_empty;
    pthread_cond_t  queue_not_full;
    pthread_t       threads[THREAD_POOL_SIZE];
    int shutdown;
} thread_pool_t;


thread_pool_t* create_thread_pool() {
    thread_pool_t* pool = malloc(sizeof(thread_pool_t));
    pool->task_queue = malloc(MAX_QUEUE_SIZE * sizeof(task_t));
    pool->queue_size = 0;
    pool->front = 0;
    pool->rear = 0;
    pool->shutdown = 0;

    pthread_mutex_init(&pool->queue_mutex, NULL);
    pthread_cond_init(&pool->queue_not_empty, NULL);
    pthread_cond_init(&pool->queue_not_full, NULL);
    
    return pool;
}


void* worker_thread(void* arg) {
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    while (1) {
        pthread_mutex_lock(&pool->queue_mutex);

        while (pool->queue_size == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->queue_not_empty, &pool->queue_mutex);
        }

        if (pool->shutdown) {
            pthred_mutex_unlock(&pool->queue_mutex);
            pthread_exit(NULL);
        }

        task_t task = pool->task_queue[pool->front];
        pool->front = (pool->front + 1) % MAX_QUEUE_SIZE;
        pool->queue_size--;

        pthread_mutex_unlock(&pool->queue_mutex);
        pthread_cond_signal(&pool->queue_not_full);

        (task.function)(task.arg);
    }

    return NULL;
}


void add_task(thread_pool_t* pool, void (*function)(void*), void* arg) {
    pthread_mutex_lock(&pool->queue_mutex);

    while (pool->queue_size == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_mutex);
    }

    task_t task = {function, arg};
    pool->task_queue[pool->rear] = task;
    pool->rear = (pool->rear + 1) % MASK_QUEUE_SIZE;
    pool->queue_size++;

    pthread_mutex_unlock(&pool->queue_mutex);
    pthread_cond_signal(&pool->queue_not_empty);
}


void start_thread_pool(thread_pool_t* pool) {
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }
}


void destroy_thread_pool(thread_pool_t* pool) {
    pthread_mutex_lock(&pool->queue_mutex);
    pool->shutdown = 1;
    pthread_mutex_unlock(&pool->queue_mutex);

    pthread_cond_broadcast(&pool->queue_not_empty);

    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_cond_destroy(&pool->queue_not_full);

    free(pool->task_queue);
    free(pool);
}






















