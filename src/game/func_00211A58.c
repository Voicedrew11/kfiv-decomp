#include "kf_script.h"

extern u8 *D_00433340[];

/* Writes a game variable: `code` is the block's index in D_00433340 (low byte) and the value's type above it. */
void func_00211A58(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    dest = D_00433340[code & 0xFF] + offset;
    type = code >> 8;
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
