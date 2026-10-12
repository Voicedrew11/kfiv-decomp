#include "common.h"
#include "kf_file.h"
#include "kf_load.h"

extern int D_00446858;

int sceCdBreak(void);
void func_00102228(void);

/* Stop a file load job: break any read in flight, close its handle and reset the step counter. */
void func_0019F558(LoadJob *job) {
    if (job->file_handle >= 0) {
        sceCdBreak();
        func_001024B0(job->file_handle);
        func_00102228();
    }
    job->file_handle = -1;
    D_00446858 = 0;
}
