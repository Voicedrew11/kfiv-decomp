#include "common.h"
#include "kf_load.h"

extern int func_001CD020(int, int, int, int, int, int, int);
extern int D_00446858;

int func_0019F498(IopLoadJob *job) {
    int *ctl = job->ctl;
    int result;

    result = func_001CD020(job->unk18, ctl[1], ctl[4], ctl[0], ctl[3], 0, job->unk1C);
    job->result = result;
    if (result >= 0) {
        D_00446858 += 1;
    }
    return 0;
}
