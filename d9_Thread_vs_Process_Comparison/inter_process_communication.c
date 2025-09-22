#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void demonstrate_pipe_communication() {
    int pipe_fd[2];
    char buffer[BUFFER_SIZE];
    
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        close(pipe_fd[1]);  // Close write end
        
        ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE);
        printf("Child received: %s\n", buffer);
        
        close(pipe_fd[0]);
        exit(0);
    } else {
        // Parent process
        close(pipe_fd[0]);  // Close read end
        
        const char* message = "Hello from parent!";
        write(pipe_fd[1], message, strlen(message) + 1);
        printf("Parent write: %s\n", message);

        close(pipe_fd[1]);
        wait(NULL);
    }
}

int main() {
    demonstrate_pipe_communication();
    return 0;
}

