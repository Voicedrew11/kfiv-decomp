#include "kf_movie.h"

/* Empties the ring of decoded pictures. */
s32 func_0021E578(MovieOutBuffer *buffer) {
    buffer->count = 0;
    buffer->write = 0;
    return 0;
}
