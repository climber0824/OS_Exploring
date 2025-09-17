#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 8192

typedef struct {
    ucontext_t context;
    int id;
} Process;

void func1(void) {
    printf("Process 1 executing\n");
}

void func2(void) {
    printf("Process 2 executing\n");
}

void context_switch(Process* curr_process, Process* next_process) {
    swapcontext(&curr_process->context, &next_process->context);
}

int main() {
    Process p1, p2;
    char stack1[STACK_SIZE], stack2[STACK_SIZE];

    // Initialize p1
    getcontext(&p1.context);
    p1.context.uc_stack.ss_sp = stack1;
    p1.context.uc_stack.ss_size = STACK_SIZE;
    p1.context.uc_link = NULL;
    p1.id = 1;
    makecontext(&p1.context, func1, 0);

    // Initialize p2
    getcontext(&p2.context);
    p2.context.uc_stack.ss_sp = stack2;
    p2.context.uc_stack.ss_size = STACK_SIZE;
    p2.context.uc_link = NULL;
    p2.id = 2;
    makecontext(&p2.context, func2, 0);

    // Perform context switches
    printf("Start context switching demo\n");
    context_switch(&p1, &p2);
    context_switch(&p2, &p1);

    return 0;
}
