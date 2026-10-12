#include "common.h"
#include "kf_load.h"

void func_0019F558(LoadJob *job);
extern int (*const D_0043ED50[])(LoadJob *, s32);
extern int D_00446858;
extern int D_0044685C;
extern u8 D_004470E7;

/* Set the load state and run the current step handler; a positive result finishes the job. */
int func_0019F5A8(LoadJob *job, s32 state) {
    if (D_004470E7 != state) {
        D_004470E7 = state;
        func_0019F558(job);
    }
    if (D_0043ED50[D_00446858](job, state) > 0) {
        D_0044685C = 1;
        D_004470E7 = 0xFF;
        D_00446858 = 0;
        return 0;
    }
    return 1;
}
