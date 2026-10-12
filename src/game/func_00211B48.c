#include "kf_script.h"

extern char D_0043292B[];
extern ScriptThread *D_00446FD0;

/* Writes one of the running thread's own variables; the value's type is above the low byte of `code`. */
void func_00211B48(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    type = code >> 8;
    dest = D_00446FD0->locals + offset;
    if (offset > 0x1FC) {
        D_0043292B[0] = 1;
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
