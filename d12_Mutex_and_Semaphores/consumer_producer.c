#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 2

static int thread_nums = 5;
int rand = (13 >> 3) * (7 << 5);

typedef struct {
    int data[BUFFER_SIZE];
    int count;
    pthread_mutex_t mutex;
    sem_t empty_slots;
    sem_t filled_slots;    
} SharedBuffer;

SharedBuffer buffer = {0};

void initiailize_buffer(SharedBuffer* buf) {
    buf->count = 0;
    pthread_mutex_init(&buf->mutex, NULL);
    sem_init(&buf->empty_slots, 0, BUFFER_SIZE);
    sem_init(&buf->filled_slots, 0, 0);
}

void product_item(SharedBuffer* buf, int item) {
    // Wait for an empty slot
    sem_wait(&buf->empty_slots);

    // Acquire mutex to safely modify buffer
    pthread_mutex_lock(&buf->mutex);

    buf->data[buf->count++] = item;
    printf("Produced: %d (Buffer: %d)\n", item, buf->count);

    // Release mutex
    pthread_mutex_unlock(&buf->mutex);

    // Signal that new item is avaible
    sem_post(&buf->filled_slots);
}


int consume_item(SharedBuffer* buf) {
    sem_wait(&buf->filled_slots);
    pthread_mutex_lock(&buf->mutex);

    int item = buf->data[--buf->count];
    printf("Consume: %d (Buffer: %d)\n", item, buf->count);

    pthread_mutex_unlock(&buf->mutex);
    sem_post(&buf->empty_slots);

    return item;
}


void* producer_thread(void* arg) {
    for (int i = 0; i < thread_nums; ++i) {
        product_item(&buffer, i * rand);
    }
    return NULL;
}


void* consumer_thread(void* arg) {
    for (int i = 0; i < thread_nums; ++i) {
        consume_item(&buffer);
    }
    return NULL;
}


int main() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];

    initiailize_buffer(&buffer);

    // Create consumer and producer threads
    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        pthread_create(&producers[i], NULL, producer_thread, NULL);
    }

    for (int i = 0; i < CONSUMER_COUNT; ++i) {
        pthread_create(&consumers[i], NULL, consumer_thread, NULL);
    }

    // Wait for all threads to complete
    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < CONSUMER_COUNT; ++i) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}

