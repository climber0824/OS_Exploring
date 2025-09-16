#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Create a new process
    pid = fork();
    if (pid < 0) {
        // Fork failed
        fprintf(stderr, "Fork failed!\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child process: PID = %d\n", getpid());
        printf("Child process: Parent PID = %d\n", getppid());
        // Execute a new program
        char *args[] = {"ls", "-a", NULL};
        execvp("ls", args);
        // If execvp returns, it must have failed
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        printf("Parent process: PID = %d\n", getpid());
        printf("Parent process: Child PID = %d\n", pid);
        // Wait for the child process to complete
        wait(NULL);
        printf("Parent process: Child has terminated\n");
    }

    return 0;
}
