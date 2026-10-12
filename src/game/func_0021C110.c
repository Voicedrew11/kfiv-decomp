#include "kf_movie.h"

/* Empties the read ring (readBufCreate). */
int func_0021C110(MovieReadBuffer *buffer) {
    buffer->put = buffer->count = 0;
    buffer->size = sizeof(buffer->data);
    return 0;
}
