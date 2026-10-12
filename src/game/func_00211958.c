#include "kf_script.h"

extern char D_0043292A[];
extern u8 D_01DFAFB0[];

/* Reads one of the variables all scripts share (the 0x400 bytes at D_01DFAFB0); the value's type is above the low byte of `code`. */
void func_00211958(ScriptArg *result, u32 code, int offset) {
    u8 *source;
    u16 type;

    type = code >> 8;
    source = D_01DFAFB0 + offset;
    if (offset > 0x3FC) {
        D_0043292A[0] = 1;
    }
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
