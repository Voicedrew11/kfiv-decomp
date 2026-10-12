#include "common.h"

extern s32 D_0044710C;
extern char D_00574348[];

void *memset(void *, s32, u32);

/* Claims the next 0x30-byte slot from a ring of slots; the index wraps at 0x100. */
void *func_001B1390(void) {
    s32 next_index;
    void *slot;

    slot = (void *) (D_00574348 + (D_0044710C * 0x30));
    memset(slot, 0, 0x30);
    next_index = D_0044710C + 1;
    D_0044710C = (next_index > -1) ? next_index : (D_0044710C + 0x100);
    D_0044710C = next_index - ((D_0044710C >> 8) << 8);
    return slot;
}
