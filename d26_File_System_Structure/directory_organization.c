#include <stdlib.h>
#include <string.h>

#include "directory_implementation.c"

// linear list implementation
typedef struct LinearDirectory {
    DirectoryEntry *entries;
    size_t count;
} LinearDirectory;


// hash table implementation
#define HASH_SIZE 1024

typedef struct HashNode{
    DirectoryEntry entry;
    struct HashNode *next;
} HashNode;


typedef struct HashDirectory {
    HashNode *buckets[HASH_SIZE];
} HashDirectory;


// B-Tree implementation
#define BTREE_ORDER 5

typedef struct BTreeNode {
    Directory *entries[BTREE_ORDER - 1];
    struct BTreeNode *children[BTREE_ORDER];
    int count;
     int leaf;
} BTreeNode;


typedef struct BTreeDirectory {
    BTreeNode *root;
} BTreeDirectory;


// hash functions for directory entries
unsigned int hash_function(const char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + *name;
        name++;
    }

    return hash % HASH_SIZE;
}


// hash directory implementation
HashDirectory *create_hash_directory() {
    HashDirectory *dir = malloc(sizeof(HashDirectory));
    if (!dir) return NULL;

    memset(dir->buckets, 0, sizeof(dir->buckets));

    return dir;
}


int hash_directory_insert(HashDirectory *dir, DirectoryEntry entry) {
    if (!dir) return -1;

    unsigned int hash = hash_function(entry.name);
    HashNode *node = malloc(sizeof(HashNode));
    if (!node) return -1;

    node->entry = entry;
    node->next = dir->buckets[hash];
    dir->buckets[hash] = node;

    return 0;
}


DirectoryEntry *hash_directory_lookup(HashDirectory *dir, const char *name) {
    if (!dir || !name) return NULL;

    unsigned int hash = hash_function(name);
    HashNode *curr = dir->buckets[hash];

    while (curr) {
        if (strcmp(curr->entry.name, name) == 0) {
            return &curr->entry;
        }
        curr = curr->next;
    }

    return NULL;
}
