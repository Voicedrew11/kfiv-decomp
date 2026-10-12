#include "kf_movie.h"

/* Accounts for `size` bytes taken out (readBufEndGet). */
int func_0021C1F8(MovieReadBuffer *buffer, int size) {
    int taken = buffer->count;

    if (size < taken) {
        taken = size;
    }
    buffer->count -= taken;
    return taken;
}
