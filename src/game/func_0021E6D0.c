#include "kf_movie.h"

/* Drops the oldest picture waiting. */
void func_0021E6D0(MovieOutBuffer *buffer) {
    if (buffer->count > 0) {
        buffer->count--;
    }
}
