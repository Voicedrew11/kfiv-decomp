#include "kf_movie.h"

extern MovieThreadState D_00437850;

/* Takes `size` bytes at the next multiple of `alignment` from the movie player's block; NULL when it is full. */
void *func_0021B4F0(u32 size, u32 alignment) {
    u32 start = (D_00437850.heap_next + alignment - 1) / alignment * alignment;
    u32 end = start + size;

    if (D_00437850.heap_end < end) {
        return NULL;
    }
    D_00437850.heap_next = end;
    return (void *)start;
}
