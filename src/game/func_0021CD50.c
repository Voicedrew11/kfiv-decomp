#include "common.h"
#include <eeregs.h>

int DIntr(void);
int EIntr(void);

/* Writes D3_CHCR with the DMA controller held (libmpeg sample's setD3_CHCR). */
void func_0021CD50(int chcr) {
    DIntr();
    *D_ENABLEW = *D_ENABLER | 0x10000;
    *D3_CHCR = chcr;
    *D_ENABLEW = *D_ENABLER & 0xFFFEFFFF;
    EIntr();
}
