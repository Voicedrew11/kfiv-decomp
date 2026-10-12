#include "kf_script.h"

extern u8 *D_00433340[];

/* Reads a game variable: `code` is the block's index in D_00433340 (low byte) and the value's type above it. */
void func_002118D8(ScriptArg *result, u32 code, int offset) {
    u8 *source;
    u16 type;

    source = D_00433340[code & 0xFF] + offset;
    type = code >> 8;
    switch (type) {
    case 0:
        result->value = *(s8 *)source;
        break;
    case 1:
        result->value = *source;
        break;
    case 2:
        result->value = *(s16 *)source;
        break;
    case 3:
        result->value = *(u16 *)source;
        break;
    case 4:
    case 5:
    case 6:
        result->value = *(s32 *)source;
        break;
    }
    result->opcode = type;
}
