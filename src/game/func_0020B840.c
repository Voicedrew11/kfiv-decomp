#include "common.h"

int FlushCache(int);
int sceGsSwapDBuff(void *, int);
int sceGsSyncV(int);
extern char D_00432D20[];
extern char D_00432F50[];
extern int D_00446F78;
extern int D_00446F7C;

/* Wait for vsync, then swap both double buffers. */
void func_0020B840(void) {
    do {

    } while (sceGsSyncV(0) == 0);
    FlushCache(0);
    sceGsSwapDBuff(D_00432D20, D_00446F78 == 0);
    FlushCache(0);
    sceGsSwapDBuff(D_00432F50, D_00446F7C == 0);
}
