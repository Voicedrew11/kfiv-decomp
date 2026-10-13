#include "kf_game.h"
#include "kf_memory.h"

extern char D_01BFAF80[];
extern ClockControl D_00432820;
extern s32 D_00446F38;


/* Lays out the memory areas, then resets the control block's four bytes at 0x208. */
void func_0020B6A0(void) {
    Memory_Init();
    D_00432820.unk208 = 0;
    D_00432820.unk209 = 0;
    D_00432820.unk20A = 0;
    D_00432820.unk20B = 2;
    D_00446F38 = (s32)D_01BFAF80;
}
