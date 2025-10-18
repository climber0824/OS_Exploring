#include <fcntl.h>
#include <unistd.h>

#define MAX_FD 1024


typedef struct FDTable{
    int fd_arr[MAX_FD];
    int count;
} FDTable;


FDTable *fd_table_create() {
    FDTable *table = malloc(sizeof(FDTable));
    if (table) {
        memset(table->fd_arr, -1, sizeof(table->fd_arr));
        table->count = 0;
    }

    return table;
}


int fd_table_add(FDTable *table, int fd) {
    if (!table || fd >= MAX_FD) return -1;

    for (int i = 0; i < MAX_FD; ++i) {
        if (table->fd_arr[i] == -1) {
            table->fd_arr[i] = fd;
            table->count++;
            return i;
        }
    }

    return -1;
}


int fd_table_remove(FDTable *table, int idx) {
    if (!table || idx < 0 || idx >= MAX_FD) return -1;

    if (table->fd_arr[idx] != -1) {
        int fd = table->fd_arr[idx];
        table->fd_arr[idx] = -1;
        table->count--;
        return fd;
    }

    return -1;
}


int main() {
    // some code to call functions above
    FDTable *fd_table = fd_table_create();
    fd_table_add(fd_table, 200);
    fd_table_remove(fd_table, 200);

    return 0;
}
