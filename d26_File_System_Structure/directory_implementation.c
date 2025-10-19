#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_NAME_LEN 255
#define MAX_PATH_LEN 4096
#define MAX_ENTRIES  1024


typedef enum EntryType {
    ENTRY_TYPE_FILE,
    ENTRY_TYPE_DIRECTORY,
    ENTRY_TYPE_SYMLINK,
    ENTRY_TYPE_SPECIAL
} EntryType;


typedef struct DirectoryEntry {
    char name[MAX_NAME_LEN];
    EntryType type;
    size_t size;
    time_t created_time;
    time_t modified_time;
    mode_t permissions;
    ino_t inode_num;
    struct DirectoryEntry *parent;
} DirectoryEntry;


typedef struct Directory {
    DirectoryEntry *entries;
    size_t entry_cnt;
    size_t capacity;
    char path[MAX_PATH_LEN];
} Directory;


// create a new directory struct
Directory *init_directory(const char *path) {
    Directory *dir = malloc(sizeof(Directory));
    if (!dir) return NULL;

    dir->entries = malloc(sizeof(DirectoryEntry) * MAX_ENTRIES);
    if (!dir->entries) {
        free(dir);
        return NULL;
    }

    dir->entry_cnt = 0;
    dir->capacity = MAX_ENTRIES;
    strncpy(dir->path, path, MAX_PATH_LEN - 1);

    // create "." and ".." entries
    DirectoryEntry *curr = &dir->entries[dir->entry_cnt++];
    strncpy(curr->name, ".", MAX_NAME_LEN);
    curr->type = ENTRY_TYPE_DIRECTORY;
    curr->created_time = time(NULL);
    curr->modified_time = curr->created_time;
    curr->parent = NULL;

    DirectoryEntry *parent = &dir->entries[dir->entry_cnt++];
    strncpy(curr->name, "..", MAX_NAME_LEN);
    curr->type = ENTRY_TYPE_DIRECTORY;
    curr->created_time = time(NULL);
    curr->modified_time = curr->created_time;
    curr->parent = NULL;

    return dir;
}


// add entry to directory
int add_directory_entry(Directory *dir, const char *name, EntryType type) {
    if (!dir || !name || dir->entry_cnt >= dir->capacity) {
        return -1;
    }

    DirectoryEntry *entry = &dir->entries[dir->entry_cnt];
    strncpy(entry->name, name, MAX_NAME_LEN - 1);
    entry->type = type;
    entry->created_time = time(NULL);
    entry->modified_time = entry->created_time;
    entry->parent = &dir->entries[0];   // point to "." entry

    dir->entry_cnt++;

    return 0;
}


// find entry in directory
DirectoryEntry *find_entry(Directory *dir, const char *name) {
    if (!dir || !name) return NULL;

    for (size_t i = 0; i < dir->entry_cnt; ++i) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            return &dir->entries[i];
        }
    }

    return NULL;
}
