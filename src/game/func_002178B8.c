#include "common.h"

extern char D_01E0DB00[];

void func_002178B8(s32 slot_x, s32 slot_y, u8 *src) {
    char *slot;
    char *dst;
    s32 i;

    slot = D_01E0DB00 + ((slot_x * 4 + slot_y) * 0x1C0);
    dst = slot + 0x174;
    for (i = 0; i < 6; i++) {
        dst[i] = src[i];
    }
}
