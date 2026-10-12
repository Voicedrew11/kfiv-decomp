#include "common.h"
#include "kf_load.h"
#include "kf_file.h"

extern void func_00102228(void);
extern int D_00446858;
extern int D_0044685C;

/* Once the read on the job file finishes, close it. */
int func_0019EF00(LoadJob *job) {
    if (func_00102790(job->file_handle, 1) == 0) {
        func_001024B0(job->file_handle);
        func_00102228();
        job->file_handle = -1;
        D_0044685C = 1;
        D_00446858 += 1;
        return 0;
    }
    return -1;
}
