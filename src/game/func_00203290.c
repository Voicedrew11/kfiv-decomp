#include "common.h"
#include "kf_game.h"

extern PoolSlot D_0042A8F0[];
extern void *D_0042B740[];
extern char D_0042B760[];
extern void *D_00446DF0;

void func_00203290(void) {
    PoolSlot **link;
    PoolSlot *next_slot;
    s32 i;

    D_0042B740[0] = D_0042B760;
    D_00446DF0 = D_0042A8F0;
    link = &D_0042A8F0[0].next;
    next_slot = D_0042A8F0 + 1;
    for (i = 0x1F; i != 0; i--) {
        *link = next_slot;
        link += sizeof(PoolSlot) / sizeof(PoolSlot *); // match: steps the next pointer one slot on
        next_slot++;
    }
    D_0042A8F0[31].next = NULL;
}
