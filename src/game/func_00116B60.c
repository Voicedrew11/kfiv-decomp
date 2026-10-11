#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

void func_00116B60(int *arg0) {
    u32 *p0;
    u32 *p;

    if (arg0 != 0) {
        p0 = p = func_00109990(2);
        *p++ = *arg0 | 0x30000000;
        *p++ = (u32)arg0 + 0x10;
        *p++ = 0;
        *p++ = 0;
        func_00109650(p0, p);
    }
}
