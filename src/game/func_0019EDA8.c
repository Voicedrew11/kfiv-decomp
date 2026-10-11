#include "kf_load.h"
#include "kf_file.h"

extern void func_00102228(void);
extern int func_001CE510(u32);
extern int func_001CE408(int);
extern int func_001CD6E0(u32);
extern int D_00446858;
extern int D_0044685C;

/* Once the read on the job file finishes, close it and release the job buffers. */
int func_0019EDA8(LoadJob *job) {
    if (func_00102790(job->file_handle, 1) != 0) {
        return -1;
    }
    func_001024B0(job->file_handle);
    func_00102228();
    job->file_handle = -1;
    func_001CE510(job->unk14);
    func_001CE408(job->unk14);
    func_001CD6E0(job->unk10);
    D_0044685C = 1;
    D_00446858 += 1;
    return 0;
}
