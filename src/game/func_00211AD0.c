#include "kf_script.h"

extern char D_0043292A[];
extern u8 D_01DFAFB0[];

/* Writes one of the variables all scripts share; the value's type is above the low byte of `code`. */
void func_00211AD0(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    type = code >> 8;
    dest = D_01DFAFB0 + offset;
    if (offset > 0x3FC) {
        D_0043292A[0] = 1;
    }
    switch (type) {
    case 0:
    case 1:
        *dest = source->value;
        break;
    case 2:
    case 3:
        *(u16 *)dest = source->value;
        break;
    case 4:
    case 5:
    case 6:
        *(s32 *)dest = source->value;
        break;
    }
}
