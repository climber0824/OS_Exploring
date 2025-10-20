#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "directory_operations.c"
#include "directory_implementation.c"

typedef struct DirectorySystem {
    DirectoryTree *tree;
    DirectoryCache *cache;
    DirectoryJournal *journal;
    DirectoryOperations ops;
} DirectorySystem;


typedef struct DirectoryStats {
    size_t cache_hits;
    size_t cache_misses;
    size_t total_operations;
    struct timespec total_lookup_time;
    struct timespec total_create_time;
    struct timespec total_delete_time;
} DirectoryStats;


DirectorySystem *create_directory_system(const char *root_path) {
    DirectorySystem *system = malloc(sizeof(DirectorySystem));
    if (!system) return NULL;

    system->tree = create_directory_tree();
    system->cache = create_directory_cache();
    system->journal = create_journal("directory.journal");

    if (!system->tree || ! system->cache || !system->journal) {
        if (system->tree) free(system->tree);
        if (system->cache) free(system->cache);
        if (system->journal) free(system->journal);
        free(system);

        return NULL;
    }

    system->ops.create = create_directory_impl;
    system->ops.remove = remove_directory_impl;
    system->ops.rename = rename_directory_impl;
    system->ops.lookup = lookup_directory_impl;
    system->ops.list   = list_directory_impl;
    
    return system;
}


void update_stats(DirectorySystem *stats, struct timespec start, struct timespec end,
                  int operation_type) {
    
    struct timespec diff;
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_nsec = end.tv_nsec - start.tv_nsec;

    if (diff.tv_nsec < 0) {
        diff.tv_sec--;
        diff.tv_nsec += 1e9L;
    }

    switch (operation_type)
    {
    case OPERATION_LOOKUP:
        timespec_add(&stats->total_lookup_time, &diff);
        break;
    case OPERATION_CREATE:
        timespec_add(&stats->total_create_time, &diff);
        break;
    case OPERATION_DELETE:
        timespec_add(&stats->total_delete_time, &diff);
        break;
    
    stats->total_operations++;
}


int main() {
    DirectorySystem *sys = create_directory_system("/root");
    if (!sys) {
        fprintf(stderr, "Failed to create directory system\n");
        return 1;
    }

    // create a directory
    if (sys->ops.create("/root/test", 0755) < 0) {
        fprintf(stderr, "Failed to create directory: %s\n", strerror(errno));
        return 1;
    }

    // list directory contents
    DirectoryEntry *entries;
    size_t count;
    if (sys->ops.list("/root", &entries, &count) < 0) {
        fprintf(stderr, "Failed to list directory: %s\n", stderr(errno));
        return 1;
    }

    for (size_t i = 0; i < count; ++i) {
        printf("%s\n", entries[i].name);
    }

    return 0;
}


