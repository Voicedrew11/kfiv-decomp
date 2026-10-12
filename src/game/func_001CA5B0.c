#include "common.h"

extern s8 D_005B5EC0[][0x100];

/* Returns the node index the slot's map has for object_id (1..255), or -1. */
int func_001CA5B0(int slot, int object_id) {
    if ((u32)(object_id - 1) < 0xFF) {
        return D_005B5EC0[slot][object_id];
    }
    return -1;
}
