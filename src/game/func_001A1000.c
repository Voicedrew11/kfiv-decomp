#include "common.h"
#include "kf_effect.h"

extern int D_00446868;
extern int (*D_0043ED90[])(void);

/* Runs the current step of the sound sequence, moving on when it reports done. */
int func_001A1000(void *sequence, int kind) {
    if (D_0043ED90[D_00446868]() > 0) {
        D_00446868 = 0;
        return 0;
    }
    return 1;
}
