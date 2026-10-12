#include "kf_movie.h"

/* Gives the oldest waiting data and returns how many bytes wait (readBufBeginGet). */
int func_0021C1B0(MovieReadBuffer *buffer, u8 **src) {
    if (buffer->count != 0) {
        *src = buffer->data + (buffer->put - buffer->count + buffer->size) % buffer->size;
    }
    return buffer->count;
}
