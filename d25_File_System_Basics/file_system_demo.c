#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "error_handling.c"


#define FS_MAX_PATH 4096
#define FS_MAX_FILES 1024


typedef struct FSFile {
    int fd;
    char path[FS_MAX_PATH];
    mode_t mode;
    off_t size;
    time_t atime;
    time_t mtime;
    time_t ctime;
} FSFile;


typedef struct FileSystem {
    char root[FS_MAX_PATH];
    FSFile files[FS_MAX_FILES];
    int file_count;
    FSContext error_ctx;
} FileSystem;


FileSystem *fs_create(const char *root) {
    FileSystem *fs = malloc(sizeof(FileSystem));
    if (!fs) return NULL;

    memset(fs, 0, sizeof(FileSystem));
    strncpy(fs->root, root, FS_MAX_PATH - 1);

    // create root directory if it doesn't exist
    if (mkdir(root, 0755) < 0 && errno != EEXIST) {
        fs_set_error(&fs->error_ctx, FS_ERROR_IO, "Failed to create root directory");
        free(fs);
        return NULL;
    }

    return fs;
}


int fs_create_file(FileSystem *fs, const char *path, mode_t mode) {
    if (!fs || !path) {
        fs_set_error(&fs->error_ctx, FS_ERROR_INVALID, "Invalid parameters");
        return -1;
    }

    char full_path[FS_MAX_PATH];
    snprintf(full_path, FS_MAX_PATH, "%s/%s", fs->root, path);

    int fd = open(full_path, O_CREAT | O_RDWR, mode);
    if (fd < 0) {
        fs_set_error(&fs->error_ctx, FS_ERROR_IO, "Failed to create file");
        return -1;
    }

    // add to file table
    if (fs->file_count >= FS_MAX_FILES) {
        close(fd);
        fs_set_error(&fs->error_ctx, FS_ERROR_NOSPACE, "File table full");
        return -1;
    }

    FSFile *file = &fs->files[fs->file_count++];
    file->fd = fd;
    strncpy(file->path, path, FS_MAX_PATH - 1);
    file->mode = mode;

    struct stat st;
    if (fstat(fd, &st) == 0) {
        file->size = st.st_size;
        file->atime = st.st_atime;
        file->mtime = st.st_mtime;
        file->ctime = st.st_ctime;
    }

    return fs->file_count - 1;
}


int main() {
    FileSystem *fs = fs_create("/tmp/testfs");
    if (!fs) {
        fprintf(stderr, "Failed to create file system\n");
        return 1;
    }

    int file_idx = fs_create_file(fs, "test.txt", 0644);
    if (file_idx < 0) {
        fprintf(stderr, "Error: %s\n", fs_error_string(&fs->error_ctx));
        return 1;
    }

    printf("Created file at index: %d\n", file_idx);

    return 0;
}
