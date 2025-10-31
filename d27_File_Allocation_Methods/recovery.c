/*
Recovery and consistency mechanisms ensure that 
the file system remains in a consistent state even after system crashes or power failures. 
This includes implementing journaling, checksums, and recovery procedures.

The implementation must handle both metadata and data consistency, 
providing mechanisms to detect and recover from corruption while maintaining acceptable performance levels.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct RecoveryManager {
    struct {
        int transaction_id;
        int block_num;
        void *old_data;
        void *new_data;
        int data_size;
    } *journal_entries;

    int journal_capacity;
    int journal_count;
    int checkpoint_interval;
    time_t last_checkpoint;
} RecoveryManager;


RecoveryManager *init_recovery_manager(int journal_size, int checkpoint_interval) {
    RecoveryManager *manager = malloc(sizeof(RecoveryManager));
    if (!manager) return NULL;

    manager->journal_entries = malloc(journal_size * 
                                    sizeof(*manager->journal_entries));
    if (!manager->journal_entries) {
        free(manager);
        return NULL;
    }               

    manager->journal_capacity = journal_size;
    manager->journal_count = 0;
    manager->checkpoint_interval = checkpoint_interval;
    manager->last_checkpoint = time(NULL);

    return manager;                     
}


// log a transation
int log_transaction(RecoveryManager *manager, int block_num,
                    void *old_data, void *new_data, int size) {

    if (manager->journal_count >= manager->journal_capacity) {
        // force checkpoint if journal if full
        create_checkpoint(manager);
    }

    int entry_idx = manager->journal_count++;
    manager->journal_entries[entry_idx].transaction_id = entry_idx;
    manager->journal_entries[entry_idx].block_num = block_num;

    manager->journal_entries[entry_idx].old_data = malloc(size);
    manager->journal_entries[entry_idx].new_data = malloc(size);

    if (!manager->journal_entries[entry_idx].old_data ||
        !manager->journal_entries[entry_idx].new_data) {
        return -1;
    }
    
    memcpy(manager->journal_entries[entry_idx].old_data, old_data, size);
    memcpy(manager->journal_entries[entry_idx].new_data, new_data, size);
    manager->journal_entries[entry_idx].data_size = size;

    return entry_idx;
}


// flush and clear journal
void create_checkpoint(RecoveryManager *manager) {
    if (manager->journal_count == 0) {
        printf("[Checkpoint] No pending transations.\n");
        manager->last_checkpoint = time(NULL);
        return;
    }

    printf("\n[Checkpoint] Creating checkpoint...\n");
    printf(" Total transactions: %d\n", manager->journal_count);

    // simulate flushing journal entries
    for (int i = 0; i < manager->journal_count; i++) {
        printf("    -> Committing Transaction #%d | Block %d | Size %d bytes\n",
                manager->journal_entries[i].transaction_id,
                manager->journal_entries[i].block_num,
                manager->journal_entries[i].data_size);

        free(manager->journal_entries[i].old_data);
        free(manager->journal_entries[i].new_data);
    }

    // reset journal state
    manager->journal_count = 0;
    manager->last_checkpoint = time(NULL);
    printf("Checkpoint complete at %s\n", ctime(&manager->last_checkpoint));
}


void destroy_recovery_manager(RecoveryManager *manager) {
    for (int i = 0; i < manager->journal_count; i++) {
        free(manager->journal_entries[i].old_data);
        free(manager->journal_entries[i].new_data);
    }
    free(manager->journal_entries);
    free(manager);
}


// test driver
int main() {
    RecoveryManager *rm = init_recovery_manager(3, 5);

    char old_block[16], new_block[16];
    strcpy(old_block, "OLD_DATA");
    strcpy(new_block, "NEW_DATA");

    // log multiple transations
    for (int i = 0; i < 7; i++) {
        sprintf(old_block, "OLD_%d", i);
        sprintf(new_block, "NEW_%d", i);
        log_transaction(rm, i, old_block, new_block, strlen(new_block) + 1);
    }

    create_checkpoint(rm);
    destroy_recovery_manager(rm);

    return 0;
}
