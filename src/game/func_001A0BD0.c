#include "common.h"
#include "kf_effect.h"

extern int D_00446864;
extern int (*D_0043ED88[])(void);

/* Runs the current step of the sound sequence, moving on when it reports done. */
int func_001A0BD0(void *sequence, int index) {
    if (D_0043ED88[D_00446864]() > 0) {
        D_00446864 = 0;
        return 0;
    }
    return 1;
}
