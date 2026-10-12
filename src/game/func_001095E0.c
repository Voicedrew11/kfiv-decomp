#include "common.h"

extern int D_00446748;          /* which of the two packet buffers is being filled */
extern int D_0044674C;
extern int D_00446750;          /* bytes in both buffers together */
extern void *D_00447018[2];
extern void *D_00447020[2];
extern void *D_00447030[2];
extern void *D_00447038;        /* next free quadword */
extern int D_0044703C;

void *memset(void *, int, u32);

/* Starts filling the other packet buffer. */
void func_001095E0(void) {
    D_00446748 = D_00446748 == 0;
    D_00447018[D_00446748] = NULL;
    D_00447020[D_00446748] = NULL;
    D_00447038 = D_00447030[D_00446748];
    memset(D_00447038, 0, D_00446750 / 2);
    D_0044674C = 0;
    D_0044703C = 0;
}
