#include "common.h"

extern s32 D_00447110;
extern char D_00577348[];

void *memset(void *, s32, u32);

/* Claims the next 0x30-byte slot from the second ring of slots; the index wraps at 0x100. */
void *func_001B1400(void) {
    s32 next_index;
    void *slot;

    slot = (void *) (D_00577348 + (D_00447110 * 0x30));
    memset(slot, 0, 0x30);
    next_index = D_00447110 + 1;
    D_00447110 = (next_index > -1) ? next_index : (D_00447110 + 0x100);
    D_00447110 = next_index - ((D_00447110 >> 8) << 8);
    return slot;
}
