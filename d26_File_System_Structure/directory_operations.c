#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "directory_organization.c"

#ifndef CACHE_SIZE
    #define CACHE_SIZE 1024
#endif

#define JOURNAL_SIZE 1024

typedef struct DirectoryOperations {
    int (*create)(const char *path, mode_t mode);
    int (*remove)(const char *path);
    int (*rename)(const char *old_path, const char *new_path);
    DirectoryEntry* (*lookup)(const char *path);
    int (*list)(const char *path, DirectoryEntry **entries, size_t *count);
} DirectoryOperations;


typedef struct DirectoryTree {
    Directory *root;
    size_t total_dirs;
    size_t total_files;
} DirectoryTree;


typedef struct DirectoryCache {
    struct {
        char path[MAX_PATH_LEN];
        Directory *directory;
        time_t last_access;
        int dirty;
    } entries[CACHE_SIZE];

    size_t count;
} DirectoryCache;


typedef struct AccessControl {
    uid_t ower;
    gid_t group;
    mode_t permissions;
} AccessControl;


// extended access control list implementation
typedef struct ACLEntry {
    uid_t uid;
    gid_t gid;
    mode_t permissions;
    struct ACLEntry *next;
} ACLEntry;


typedef struct ExtendedACL {
    ACLEntry *entries;
    size_t count;
} ExtendedACL;


typedef struct DirectoryJournalEntry {
    enum {
        JOURNAL_CREATE,
        JOURNAL_DELETE,
        JOURNAL_MODIFY
    } operation;

    char path[MAX_PATH_LEN];
    DirectoryEntry entry;
    time_t timestamp;
} DirectoryJournalEntry;


typedef struct DirectroyJournal {
    DirectoryJournalEntry *entries;
    size_t capacity;
    size_t count;
    FILE   *joural_file;
} DirectoryJournal;


int create_directory_impl(const char *path, mode_t mode) {
    Directory *parent = get_parent_directory(path);
    if (!parent) {
        errno = ENOENT;
        return -1;
    }

    char *name = get_basename(path);
    if (find_entry(parent, name)) {
        errno = EEXIST;
        return -1;
    }

    Directory *new_dir = create_directory(path);
    if (!new_dir) {
        errno = ENOMEM;
        return -1;
    }

    return add_directory_entry(parent, name, ENTRY_TYPE_DIRECTORY);    
}


int remove_directory_impl(const char *path) {
    Directory *dir = resolve_directory(path);
    if (!dir) {
        errno = ENOENT;
        return -1;
    }

    if (dir->entry_cnt > 2) {   // more than "." and ".."
        errno = ENOTEMPTY;
        return -1;
    }

    Directory *parent = get_parent_directory(path);
    if (!parent) {
        errno = ENOENT;
        return -1;
    }

    char *name = get_basename(path);

    return remove_directory_entry(parent, name);
}


DirectoryTree *create_directory_tree() {
    DirectoryTree *tree = malloc(sizeof(DirectoryTree));
    if (!tree) return NULL;

    tree->root = create_directory("/");
    if (!tree->root) {
        free(tree);
        return NULL;
    }

    tree->total_dirs = 1;
    tree->total_files = 0;

    return tree;
}


// recursive directory traversal
void traverse_directory_tree(Directory *dir, void (*callback)(DirectoryEntry*)) {
    if (!dir || !callback) return;

    for (size_t i = 0; i < dir->entry_cnt; ++i) {
        DirectoryEntry *entry = &dir->entries[i];
        callback(entry);

        if (entry->type == ENTRY_TYPE_DIRECTORY && 
            strcmp(entry->name, ".") != 0 &&
            strcmp(entry->name, "..") != 0 ) {
            
            Directory *subdir = load_directory(entry);
            if (subdir) {
                traverse_directory_tree(subdir, callback);
            }
        }
    }   
}


DirectoryCache *create_directory_cache() {
    DirectoryCache *cache = malloc(sizeof(DirectoryCache));
    if (cache) {
        memset(cache, 0, sizeof(DirectoryCache));
    }
    
    return cache;
}


DirectoryCache *cache_lookup(DirectoryCache *cache,  const char *path) {
    if (!cache || !path) return NULL;

    for (size_t i = 0; i < cache->count; ++i) {
        if (strcmp(cache->entries[i].path, path) == 0) {
            cache->entries[i].last_access = time(NULL);

            return cache->entries[i].directory;
        }
    }

    return NULL;
}


int cache_insert(DirectoryCache *cache, const char *path, Directory *dir) {
    if (!cache || !path || !dir) return -1;

    // evict least recently used entry if cache is full
    if (cache->count >= CACHE_SIZE) {
        size_t lru_idx = 0;
        time_t oldest_time = cache->entries[0].last_access;

        for (size_t i = 0; i < CACHE_SIZE; ++i) {
            if (cache->entries[i].last_access < oldest_time) {
                oldest_time = cache->entries[i].last_access;
                lru_idx = i;
            }
        }

        // flush dirty entry before eviction
        if (cache->entries[lru_idx].dirty) {
            flush_directory(cache->entries[lru_idx].directory);
        }

        free_directory(cache->entries[lru_idx].directory);
        cache->count--;
    }

    size_t idx = cache->count++;
    strncpy(cache->entries[idx].path, path, MAX_PATH_LEN - 1);
    cache->entries[idx].directory = dir;
    cache->entries[idx].last_access = time(NULL);
    cache->entries[idx].dirty = 0;

    return 0;
}


int check_directory_access(Directory *dir, uid_t uid, gid_t gid, int desired_access) {
    if (!dir) return -1;

    AccessControl *acl = &dir->entries[0].access_control;

    // owner access
    if (uid == acl->ower) {
        if ((desired_access & R_OK) && !(acl->permissions & S_IRUSR)) return -1;
        if ((desired_access & W_OK) && !(acl->permissions & S_IWUSR)) return -1;
        if ((desired_access & X_OK) && !(acl->permissions & S_IXUSR)) return -1;
        return 0;
    }

    // group access
    if (gid == acl->group) {
        if ((desired_access & R_OK) && !(acl->permissions & S_IRGRP)) return -1;
        if ((desired_access & W_OK) && !(acl->permissions & S_IWGRP)) return -1;
        if ((desired_access & X_OK) && !(acl->permissions & S_IXGRP)) return -1;
        return 0;
    }

    // other access
    if ((desired_access & R_OK) && !(acl->permissions & S_IROTH)) return -1;
    if ((desired_access & W_OK) && !(acl->permissions & S_IWOTH)) return -1;
    if ((desired_access & X_OK) && !(acl->permissions & S_IXOTH)) return -1;
    
    return 0;
}


int add_acl_entry(ExtendedACL *acl, uid_t uid, gid_t gid, mode_t permissions) {
    ACLEntry *entry = malloc(sizeof(ACLEntry));
    if (!entry) return -1;

    entry->uid = uid;
    entry->gid = gid;
    entry->permissions = permissions;
    entry->next = acl->entries;
    acl->entries = entry;
    acl->count++;

    return 0;    
}


int check_extended_access(ExtendedACL *acl, uid_t uid, gid_t gid, int desired_access) {
    ACLEntry *curr = acl->entries;

    while (curr) {
        if (curr->uid == uid || curr->gid == gid) {
            if ((desired_access & R_OK) && !(curr->permissions & S_IRUSR)) return -1;
            if ((desired_access & W_OK) && !(curr->permissions & S_IWUSR)) return -1;
            if ((desired_access & X_OK) && !(curr->permissions & S_IXUSR)) return -1;
            return 0;
        }
        curr = curr->next;
    }

    return -1;
}


DirectoryJournal *create_journal(const char *journal_path) {
    DirectoryJournal *journal = malloc(sizeof(DirectoryJournal));
    if (!journal) return NULL;

    journal->entries = malloc(sizeof(DirectoryJournalEntry) * JOURNAL_SIZE);
    if (!journal->entries) {
        free(journal);
        return NULL;
    }

    journal->capacity = 1024;
    journal->count = 0;
    journal->joural_file = fopen(journal_path, "a+b");

    return journal;
}


int log_directory_operation(DirectoryJournal *journal,
                            int operation,
                            const char *path,
                            DirectoryEntry *entry) {
    if (journal->count >= journal->capacity) {
        // flush oldest entries to dist
        flush_joural_entries(journal);
    }

    DirectoryJournalEntry *j_entry = &journal->entries[journal->count++];
    j_entry->operation = operation;
    strncpy(j_entry->path, path, MAX_PATH_LEN - 1);
    if (entry) {
        j_entry->entry = *entry;
    }
    j_entry->timestamp = time(NULL);

    // write to journal file immediately
    fwrite(j_entry, sizeof(DirectoryJournalEntry), 1, journal->joural_file);
    fflush(journal->joural_file);

    return 0;
}


int recover_directory_state(DirectoryJournal *journal, Directory *dir) {
    // read journal from disk
    fseek(journal->joural_file, 0, SEEK_SET);
    DirectoryJournalEntry entry;

    while (fread(&entry, sizeof(DirectoryJournalEntry), 1, journal->joural_file) == 1) {
        switch (entry.operation)
        {
        case JOURNAL_CREATE:
            add_directory_entry(dir, entry.entry.name, entry.entry.type);            
            break;
        case JOURNAL_DELETE:
            remove_directory_entry(dir, entry.entry.name);
            break;
        case JOURNAL_MODIFY:
            update_directory_entry(dir, &entry.entry);        
            break;
        }
    }

    return 0;
}
