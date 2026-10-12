#include "kf_movie.h"

/* Accounts for `size` bytes written at the put position (readBufEndPut). */
int func_0021C168(MovieReadBuffer *buffer, int size) {
    int accepted = buffer->size - buffer->count;

    if (size < accepted) {
        accepted = size;
    }
    buffer->put = (buffer->put + accepted) % buffer->size;
    buffer->count += accepted;
    return accepted;
}
