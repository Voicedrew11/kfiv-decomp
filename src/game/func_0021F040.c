#include "common.h"

/* Writes a GIF tag at address `packet`: the register list, then NLOOP, EOP, PRE, PRIM, FLG
   and NREG. Returns the address after it. */
int func_0021F040(int packet, s64 regs, s32 nreg, s32 flg, s32 prim, s32 pre, s32 eop, s32 nloop) {
    u32 *words = (u32 *)packet;

    words[0] = (eop << 15) | nloop;
    words[1] = (pre << 14) | (prim << 15) | (flg << 26) | (nreg << 28);
    words[2] = regs & 0xFFFFFFFF;
    words[3] = regs >> 32;
    return (int)(words + 4);
}
