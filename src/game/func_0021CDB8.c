#include "common.h"
#include <eeregs.h>

int DIntr(void);
int EIntr(void);

void func_0021CDB8(int chcr) {
    DIntr();
    *D_ENABLEW = *D_ENABLER | 0x10000;
    *D4_CHCR = chcr;
    *D_ENABLEW = *D_ENABLER & 0xFFFEFFFF;
    EIntr();
}
