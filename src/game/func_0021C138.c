#include "kf_movie.h"

/* Gives the place the next data goes and returns the free bytes (readBufBeginPut). */
int func_0021C138(MovieReadBuffer *buffer, u8 **dest) {
    int free_size = buffer->size - buffer->count;

    if (free_size != 0) {
        *dest = buffer->data + buffer->put;
    }
    return free_size;
}
