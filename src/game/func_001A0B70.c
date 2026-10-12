#include "common.h"
#include "kf_sound.h"

extern int D_00446864;

int func_001CBD20(void);
int func_001CD6B8(int);
int func_001CD7E0(int);
int FlushCache(int);

/* Stops the voice of a job and counts the stop; -1 if the hardware is not ready. */
int func_001A0B70(SoundJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446864 += 1;
    return 1;
}
