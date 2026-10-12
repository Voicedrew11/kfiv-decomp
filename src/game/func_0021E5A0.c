#include "kf_movie.h"

int DIntr(void);
int EIntr(void);

/* Marks the slot being written as holding a picture and moves on to the next slot. */
void func_0021E5A0(MovieOutBuffer *buffer) {
    DIntr();
    buffer->tag[buffer->write].status = MOVIE_OUT_STATUS_FULL;
    buffer->count++;
    buffer->write = (buffer->write + 1) % buffer->size;
    EIntr();
}
