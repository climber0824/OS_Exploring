#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

void secure_processing() {
    // Sensitive data in isolated process
    char* sensitive_data = mmap(NULL, 4096,
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    sprintf(sensitive_data, "SECRET_KEY=12345");
    
    // Process the data in isolation
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - isolated memory space
        printf("Child process handling sensitive data\n");
        // Process sensitive_data here
        munmap(sensitive_data, 4096);
        exit(0);
    } else {
        wait(NULL);
        munmap(sensitive_data, 4096);
    }
}
