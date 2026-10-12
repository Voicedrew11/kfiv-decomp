#include "common.h"

/* Writes a DMA tag at address `packet` (one doubleword; the other half of its quadword is
   left alone). Returns the address after the quadword. */
int func_0021EFE8(int packet, u32 spr, u32 addr, u32 irq, u32 tag_id, u32 pce, u32 qwc) {
    u64 *tag = (u64 *)packet;

    *tag = ((u64)spr << 63) | ((u64)(addr & ~0xF) << 32) | ((u64)irq << 31) | ((u64)tag_id << 28) | ((u64)pce << 26) | qwc;
    return (int)(tag + 2);
}
