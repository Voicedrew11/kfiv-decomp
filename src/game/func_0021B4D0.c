#include "kf_movie.h"

extern MovieThreadState D_00437850;

/* Gives the movie player the block of memory it allocates its buffers from. */
void func_0021B4D0(u32 start, u32 size) {
    D_00437850.heap_start = start;
    D_00437850.heap_next = start;
    D_00437850.heap_end = start + size - 1;
}
