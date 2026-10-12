#include "common.h"
#include <eeregs.h>

/* Starts the GIF DMA channel on a chain of packets. */
void func_0021F618(u32 chain) {
    *D2_TADR = chain & 0x0FFFFFFF;
    *D2_QWC = 0;
    *D2_CHCR = 0x105;
}
