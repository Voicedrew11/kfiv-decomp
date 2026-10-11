#include "kf_load.h"
#include "kf_file.h"

extern int D_003EF548[];
extern int D_00446858;
extern int D_004470F0;
extern int D_004470F4;

void func_00102218(void);
int func_00102230(void);
int func_00102900(int);
int func_001CBD20(void);
int sprintf(char *, char *, ...);

/* Open the file named by name_format and start reading it; -1 if the file layer is busy. */
int func_0019EF68(LoadJob *job, int index) {
    char name[128];
    int handle;
    int heap_top;
    int *buffers;

    if (func_00102230() != 0) {
        return -1;
    }
    if (func_001CBD20() == 0) {
        return -1;
    }
    heap_top = D_004470F4;
    D_003EF548[0] = heap_top;
    D_004470F4 = ((u32)(heap_top + 0x2003F) >> 6) << 6;
    sprintf(name, job->name_format, index);
    handle = func_00102238(name, TRUE, TRUE);
    job->file_handle = handle;
    D_004470F0 = func_00102900(handle);
    func_00102218();
    buffers = job->buffers;
    func_001025C0(job->file_handle, (void *)buffers[2], -1);
    D_00446858 += 1;
    return 0;
}
