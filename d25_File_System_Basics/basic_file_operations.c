#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define PATH_MAX 1024
#define DATA_SIZE 128

typedef struct FileOperations {
    int (*open)(const char *path, int flags);
    ssize_t (*read)(int fd, void *buffer, size_t count);
    ssize_t (*write)(int fd, const void *buffer, size_t count);
    int (*close)(int fd);
    off_t (*seek)(int fd, off_t offset, int whence);
} FileOperations;


FileOperations create_file_operations() {
    FileOperations ops = {
        .open = open,
        .read = read,
        .write = write,
        .close = close,
        .seek = lseek
    };
    return ops;
}


typedef struct FileSystem {
    FileOperations ops;
    char root_path[PATH_MAX];
} FileSystem;


typedef struct Record {
    int id;
    char data[DATA_SIZE];
} Record;


typedef struct IndexEntry {
    int key;
    long position;
} IndexEntry;


typedef struct Index {
    IndexEntry *entries;
    int size;
} Index;


FileSystem* create_file_system(const char *root) {
    FileSystem *fs = malloc(sizeof(FileSystem));
    if (!fs) return NULL;

    fs->ops = create_file_operations();
    strncpy(fs->root_path, root, PATH_MAX - 1);
    
    return fs;
}


int fs_open_file(FileSystem *fs, const char *path, int flags) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", fs->root_path, path);

    return fs->ops.open(full_path, flags);  // struct of function pointers
}


ssize_t fs_read_file(FileSystem *fs, int fd, void *buffer, size_t count) {
    return fs->ops.read(fd, buffer, count);
}


ssize_t fs_write(FileSystem *fs, int fd, const void *buffer, size_t count) {
    return fs->ops.write(fd, buffer, count);
}


int fs_close_file(FileSystem *fs, int fd) {
    return fs->ops.close(fd);
}


void sequential_access(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return;

    Record record;
    while (fread(&record, sizeof(Record), 1, file) == 1) {
        printf("Record ID: %d, Data: %s\n", record.id, record.data);
    }

    fclose(file);
}


Record* random_access(const char *filename, int record_num) {
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;

    Record *record = malloc(sizeof(record));
    if (!record) {
        fclose(file);
        return NULL;
    }

    fseek(file, record_num * sizeof(Record), SEEK_SET);
    if (fread(record, sizeof(Record), 1, file) != 1) {
        free(record);
        record = NULL;
    }

    fclose(file);
    
    return record;
}



Index *build_index(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;

    Index *idx = malloc(sizeof(Index));
    if (!idx) {
        fclose(file);
        return NULL;
    }

    // count records
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    idx->size = file_size / sizeof(Record);
    rewind(file);

    // allocate index entries
    idx->entries = malloc(idx->size * sizeof(IndexEntry));
    if (!idx->entries) {
        free(idx);
        fclose(file);
        return NULL;
    }

    // build index
    Record record;
    for (int i = 0; i < idx->size; ++i) {
        long pos = ftell(file);
        if (fread(&record, sizeof(Record), 1, file) != 1) break;
        idx->entries[i].key = record.id;
        idx->entries[i].position = pos;
    }

    fclose(file);

    return idx;
}


