#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


#define BUFFER_SIZE 4096


typedef struct Buffer {
    char data[BUFFER_SIZE];
    size_t size;
    size_t position;
    int dirty;    
} Buffer;


typedef struct BufferedFile {
    int fd;
    Buffer buffer;
    int mode;   // read or write
} BufferedFile;


BufferedFile *create_buffered_file(int fd, int mode) {
    BufferedFile *bf = malloc(sizeof(BufferedFile));
    if (!bf) return NULL;

    bf->fd = fd;
    bf->mode = mode;
    bf->buffer.size = 0;
    bf->buffer.position = 0;
    bf->buffer.dirty = 0;

    return bf;
}


int buffer_flush(BufferedFile *bf) {
    if (!bf || bf->buffer.dirty) return 0;

    ssize_t written = write(bf->fd, bf->buffer.data, bf->buffer.size);
    if (written < 0) return -1;

    bf->buffer.size = 0;
    bf->buffer.position = 0;
    bf->buffer.dirty = 0;

    return 0;
}


ssize_t buffer_read(BufferedFile *bf, void *data, size_t count) {
    if (!bf || bf->mode != O_RDONLY) return -1;

    size_t total_read = 0;
    char *dest = (char*)data;

    while (total_read < count) {
        // if buffer is empty, fill it
        if (bf->buffer.position >= bf->buffer.size) {
            bf->buffer.size = read(bf->fd, bf->buffer.data, BUFFER_SIZE);
            if (bf->buffer.size <= 0) break;
            bf->buffer.position = 0;
        }

        // copy from buffer to destination
        size_t available = bf->buffer.size - bf->buffer.position;
        size_t to_copy = (count - total_read) < available ? 
                        (count - total_read) : available;
        
        memcpy(dest + total_read,
                bf->buffer.data + bf->buffer.position,
                to_copy);

        bf->buffer.position += to_copy;
        total_read += to_copy;                
    }

    return total_read;
}


ssize_t buffer_write(BufferedFile *bf, const void *data, size_t count) {
    if (!bf || bf->mode != O_RDONLY) return -1;

    size_t total_written = 0;
    const char *src = (const char*)data;

    while (total_written < count) {
        // if buffer is full, flush it
        if (bf->buffer.size >= BUFFER_SIZE) {
            if (buffer_flush(bf) < 0) return -1;
        }

        // copy from src to buffer
        size_t available = BUFFER_SIZE - bf->buffer.size;
        size_t to_copy = (count - total_written) < available ?
                        (count - total_written) : available;
        
        memcpy(bf->buffer.data + bf->buffer.size,
                src + total_written,
                to_copy);
        
        bf->buffer.size += to_copy;
        bf->buffer.dirty = 1;
        total_written += to_copy;
    }

    return total_written;
}


int main() {
    // some code to call functions above
    BufferedFile *bf = create_buffered_file(1, 1);
    
    return 0;
}
