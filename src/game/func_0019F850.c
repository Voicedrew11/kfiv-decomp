#include "common.h"
#include "kf_load.h"

int func_001CBD20(void);
int func_001CD6B8(int);
int func_001CD7E0(int);
int FlushCache(int);
extern int D_00446858;

int func_0019F850(IopLoadJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446858 += 1;
    return 0;
}
