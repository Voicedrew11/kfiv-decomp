#include "common.h"

int FlushCache(int);
int func_0020D088(void);
int sceGsSwapDBuff(void *, int);
int sceGsSyncV(int);
extern char D_00432D20[];
extern char D_00432F50[];
extern int D_00446F78;
extern int D_00446F7C;

/* Swaps both double buffers, waiting for vsync first unless allow_late_swap is set and the frame is already late. */
void func_0020B780(int allow_late_swap) {
    int time_left = func_0020D088();

    if (allow_late_swap == 0 || time_left <= 0x4B0C7) {
        while (sceGsSyncV(0) != 1) {
        }
    }
    FlushCache(0);
    sceGsSwapDBuff(D_00432D20, D_00446F78);
    D_00446F78 = D_00446F78 == 0;
    FlushCache(0);
    sceGsSwapDBuff(D_00432F50, D_00446F7C);
    D_00446F7C = D_00446F7C == 0;
    FlushCache(0);
}
