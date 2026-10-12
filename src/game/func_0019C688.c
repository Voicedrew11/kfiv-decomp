#include "common.h"

extern u8 D_003EF6F0[][2];
extern u8 D_00446880[8];

/* Steps the slot cursor through its [low, high] range and returns the new value. */
int func_0019C688(int slot) {
    s8 index = slot;
    u8 low = D_003EF6F0[index][0];
    s8 high = D_003EF6F0[index][1];
    s8 offset = D_00446880[index] - low + 1;
    int next = offset % (high - (s8)low + 1) + low;

    D_00446880[index] = next;
    return (s8)next;
}
