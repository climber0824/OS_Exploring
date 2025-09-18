#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5
#define QUEUE_SIZE 10

typedef struct {
    int data[QUEUE_SIZE];
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} thread_safe_queue_t;

thread_safe_queue_t queue = {
    .front = 0,
    .rear = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER
};

void queue_init(thread_safe_queue_t* q) {
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void queue_destroy(thread_safe_queue_t* q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
}

void queue_push(thread_safe_queue_t* q, int value) {
    pthread_mutex_lock(&q->mutex);
    
    while ((q->rear + 1) % QUEUE_SIZE == q->front) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

int queue_pop(thread_safe_queue_t* q) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->front == q->rear) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    int value = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return value;
}

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        int value = id * 100 + i;
        queue_push(&queue, value);
        printf("Producer %d: Pushed %d\n", id, value);
        sleep(1);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        int value = queue_pop(&queue);
        printf("Consumer %d: Popped %d\n", id, value);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    queue_init(&queue);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &thread_ids[i]);
        pthread_create(&consumers[i],
        NULL, consumer, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    queue_destroy(&queue);

    printf("All threads have finished execution\n");
    return 0;
}

