#include "kf_game.h"

extern PoolSlot D_0042A8F0[32];

/* Gives the first 21 slots of the pool the file's parts, in order. */
void func_00203050(PoolPartFile *file) {
    u32 i;

    for (i = 0; i < 21; i++) {
        D_0042A8F0[i].part = &file->parts[i];
    }
}
