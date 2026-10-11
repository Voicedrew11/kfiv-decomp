#include "common.h"

int FlushCache(int);
int sceGsSetHalfOffset(void *, int, int, s16);
int sceGsSwapDBuff(void *, int);
int sceGsSyncV(int);
extern char D_00432D20[];
extern char D_00432D80[];
extern char D_00432E70[];
extern int D_00446F78;
extern int D_00446FA0;

void func_0020B708(void) {
    char *var_a0;

    D_00446FA0 = sceGsSyncV(0) == 0;
    if (D_00446F78 != 0) {
        var_a0 = D_00432E70;
    } else {
        var_a0 = D_00432D80;
    }
    sceGsSetHalfOffset(var_a0, 0x800, 0x800, (s16) D_00446FA0);
    FlushCache(0);
    sceGsSwapDBuff(D_00432D20, D_00446F78);
    D_00446F78 = D_00446F78 == 0;
}
