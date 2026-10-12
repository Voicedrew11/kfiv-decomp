#include "kf_movie.h"

s32 func_0021E658(char *);

/* Returns the tag of the oldest picture waiting, or NULL when the ring is empty. */
MovieOutTag *func_0021E668(MovieOutBuffer *buffer) {
    return func_0021E658((char *)buffer) ? NULL : buffer->tag + (buffer->write - buffer->count + buffer->size) % buffer->size;
}
