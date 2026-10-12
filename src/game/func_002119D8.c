#include "kf_script.h"

extern char D_0043292B[];

/* Reads one of the thread's own variables; the value's type is above the low byte of `code`. */
void func_002119D8(ScriptArg *result, ScriptThread *thread, u32 code, int offset) {
    u8 *source;
    u16 type;

    type = code >> 8;
    source = thread->locals + offset;
    if (offset > 0x1FC) {
        D_0043292B[0] = 1;
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
