#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define LEFT(x) ((x + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS)
#define RIGHT(x) ((x + 1) % NUM_PHILOSOPHERS)

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_t philosophers[NUM_PHILOSOPHERS];

void* philosopher_activity(void* arg) {
    int philosopher_id = *(int* )arg;
    int first_fork, second_fork;

    // Implement resource ordering to prevent deadlock    
    first_fork = philosopher_id % 2 == 0 ? LEFT(philosopher_id) : RIGHT(philosopher_id);
    second_fork = philosopher_id % 2 == 0 ? RIGHT(philosopher_id) : LEFT(philosopher_id);

    while (1) {
        // Think
        printf("Philosopher %d is thinking\n", philosopher_id);
        sleep(rand() % 3);

        // Acquire forks with deadlock prevention
        pthread_mutex_lock(&forks[first_fork]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, first_fork);

        pthread_mutex_lock(&forks[second_fork]);
        printf("Philosopher %d picked up fork %d\n", philosopher_id, second_fork);

        // Eat
        printf("Philosopher %d is eating\n", philosopher_id);
        sleep(rand() % 3);

        // Release forks
        pthread_mutex_unlock(&forks[first_fork]);
        pthread_mutex_unlock(&forks[second_fork]);        
    }

    return NULL;
}



int main() {
    int philosopher_ids[NUM_PHILOSOPHERS];

    // Initialize mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_mutex_init(&forks[i], NULL);
        philosopher_ids[i] = i;
    }

    // Create threads
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_create(&philosophers[i], NULL, philosopher_activity, &philosopher_ids[i]);
    }

    // Wait for threads
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
