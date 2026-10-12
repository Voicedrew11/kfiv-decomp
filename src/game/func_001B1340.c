#include "common.h"

extern s32 D_0044710C;
extern s32 D_00447110;
extern s32 D_00446928;
extern char D_00574348[];
extern char D_00577348[];

void *memset(void *, s32, u32);

/* Empties both rings of 0x30-byte slots. */
void func_001B1340(void) {
    D_0044710C = 0;
    memset(D_00574348, 0, 0x3000);
    D_00447110 = 0;
    memset(D_00577348, 0, 0x3000);
    D_00446928 = 3;
}
