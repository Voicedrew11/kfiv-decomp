#include "kf_load.h"
#include "kf_file.h"

extern int func_001CBD20(void);

/* Open the file named in the job, read it into the buffer at *job->buffers, then close it. */
int func_0019F6B8(LoadJob *job) {
    int file_handle;

    if (func_001CBD20() == 0) {
        return -1;
    }
    file_handle = func_00102238(job->name, 0, 1);
    func_001025C0(file_handle, (void *)*job->buffers, -1);
    func_001024B0(file_handle);
    return 1;
}
