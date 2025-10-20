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


