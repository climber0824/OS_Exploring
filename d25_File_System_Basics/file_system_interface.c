#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH_LEN 256
#define MAX_OPEN_FILES 128


typedef struct FileHandle {
    int fd;
    char path[MAX_PATH_LEN];
    off_t position;
    int flags;
} FileHandle;


typedef struct FileSystem {
    FileHandle open_files[MAX_OPEN_FILES];
    int file_count;
    char mount_point[MAX_PATH_LEN];
} FileSystem;


FileSystem *fs_init(const char *mount_point) {
    FileSystem *fs = malloc(sizeof(FileSystem));
    if (!fs) return NULL;

    memset(fs, 0, sizeof(FileSystem));
    strncpy(fs->mount_point, mount_point, MAX_PATH_LEN - 1);
    fs->file_count = 0;

    return fs;
}


int fs_open(FileSystem *fs, const char *path, int flags) {
    if (fs->file_count >= MAX_OPEN_FILES) {
        errno = EMFILE;
        return -1;
    }

    // find free handle
    int handle = -1;
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        if (fs->open_files[i].fd == 0) {
            handle = i;
            break;
        }
    }

    if (handle == -1) {
        errno = EMFILE;
        return -1;
    }

    // open the actual file
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, MAX_PATH_LEN, "%s/%s", fs->mount_point, path);

    int fd = open(full_path, flags);
    if (fd < 0) return -1;

    fs->open_files[handle].fd = fd;
    strncpy(fs->open_files[handle].path, path, MAX_PATH_LEN - 1);
    fs->open_files[handle].position = 0;
    fs->open_files[handle].flags = flags;
    fs->file_count++;

    return handle;
}


// read from file
ssize_t fs_read(FileSystem *fs, int handle, void *buffer, size_t size) {
    if (handle < 0 || handle >= MAX_OPEN_FILES || fs->open_files[handle].fd == 0) {
        errno = EBADF;
        return -1;
    }

    ssize_t bytes = read(fs->open_files[handle].fd, buffer, size);
    if (bytes > 0) {
        fs->open_files[handle].position += bytes;
    }

    return bytes;
}


// write to file
ssize_t fs_write(FileSystem *fs, int handle, const void *buffer, size_t size) {
    if (handle < 0 || handle >= MAX_OPEN_FILES || fs->open_files[handle].fd == 0) {
        errno = EBADF;
        return -1;
    }

    ssize_t bytes = write(fs->open_files[handle].fd, buffer, size);
    if (bytes > 0) {
        fs->open_files[handle].position += bytes;
    }

    return bytes;
}


// close file
int fs_close(FileSystem *fs, int handle) {
    if (handle < 0 || handle >= MAX_OPEN_FILES || fs->open_files[handle].fd == 0) {
        errno = EBADF;
        return -1;
    }

    int res = close(fs->open_files[handle].fd);
    if (res == 0) {
        memset(&fs->open_files[handle], 0, sizeof(FileHandle)); // clears the structure
        fs->file_count--;
    }

    return res;
}


int main() {
    // some code to call functions above
    const char *mount_point = "./";
    FileSystem *fs = fs_init(mount_point);
    

    return 0;
}
