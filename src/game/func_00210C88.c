#include "kf_script.h"

extern char D_00432921[];

/* Index of the thread's first free command; sets an error flag and gives 0 when all 16 are busy. */
int func_00210C88(ScriptThread *thread) {
    int i;

    for (i = 0; i < 16; i++) {
        if (thread->commands[i].active == 0) {
            return i;
        }
    }
    if (i == 16) {
        D_00432921[0] = 1;
        return 0;
    }
    // match: no return here (never reached: the loop only ends with i == 16)
}
