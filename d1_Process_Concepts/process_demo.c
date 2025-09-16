#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t process_id = getpid();   // Current process ID
    pid_t parent_process_id = getppid();  // Parent process ID

    printf("Current Process ID: %d\n", process_id);
    printf("Parent Process ID: %d\n", parent_process_id);
    
    // pid_t process_id = getpid();
    // pid_t parent_process_id = getppid();

    printf("current PID: %d\n", process_id);
    printf("parent PID: %d\n", parent_process_id);
    return 0;
}


