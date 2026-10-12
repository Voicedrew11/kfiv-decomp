#include "common.h"

extern s32 D_00420BE0[][4];

s32 func_001D0588(u16 *flags, s32 slot) {
    u32 value;

    if (!(*flags & 0xF000)) {
        if (!(*flags & 0x800)) {
            return D_00420BE0[slot][0];
        }
        value = D_00420BE0[slot][0];
        return (s32)(value + (value >> 31)) >> 1;
    }
    return 0;
}
