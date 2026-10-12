#include "common.h"

/* Writes one GS register setting at address `packet` (64-bit value, then register number,
   as four words) and returns the address for the next one. */
int func_0021F098(int packet, s32 reg, s64 value) {
    u32 *words = (u32 *)packet;

    words[0] = value & 0xFFFFFFFF;
    words[1] = value >> 32;
    words[2] = reg;
    words[3] = 0;
    return (int)(words + 4);
}
