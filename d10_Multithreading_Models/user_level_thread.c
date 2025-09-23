// Simple user-level thread implementation

#define STACK_SIZE 1024
#define THREAD_READY 1

typedef struct {
    void* stack;
    void* stack_pointer;
    void (*function)(void*);
    void* arg;
    int state;
} user_thread_t;

user_thread_t* create_user_thread(void (*func)(void*), void* arg) {
    user_thread_t* thread = malloc(sizeof(user_thread_t));
    thread->stack = malloc(STACK_SIZE);
    thread->stack_pointer = thread->stack + STACK_SIZE;
    thread->function = func;
    thread->arg = arg;
    thread->state = THREAD_READY;
    return thread;
}
