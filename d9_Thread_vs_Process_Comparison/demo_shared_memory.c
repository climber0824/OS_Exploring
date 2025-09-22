#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SHM_NAME "/my_shm"
#define SHM_SIZE 1024

void demonstrate_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);
    
    void* shm_ptr = mmap(NULL, SHM_SIZE,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED, shm_fd, 0);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        sleep(1);  // Ensure parent writes first
        printf("Child read: %s\n", (char*)shm_ptr);
        exit(0);
    } else {
        // Parent process
        sprintf(shm_ptr, "Hello from shared memory!");
        printf("Parent write: %s\n", (char* )shm_ptr);
        wait(NULL);
        
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
    }
}

int main() {
    demonstrate_shared_memory();
    return 0;
}

