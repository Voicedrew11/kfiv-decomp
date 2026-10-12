#include "common.h"

/* the vblank handler changes these two */
extern volatile s32 D_00437824[];
extern volatile s32 D_00437828[];
extern s32 D_00437A4C[];

int sceGsSyncV(int);

/* Waits for a vertical blank that starts the other field, then turns the movie's vblank handler on. */
void func_0021F7E8(s32 field) {
    while (sceGsSyncV(0) == field) {
    }
    D_00437A4C[0] = 0;
    D_00437824[0] = 1;
    D_00437828[0] = 0;
}
