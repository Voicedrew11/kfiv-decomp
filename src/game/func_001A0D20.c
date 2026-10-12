#include "common.h"
#include "kf_sound.h"

extern char D_004470E2;
extern int D_00446868;

int func_001CBAC0(void);
int func_001CD020(int, int, int, int, int, int, int);

/* Starts a sound job; -1 while the previous one is still being cleared. */
int func_001A0D20(SoundJob *job) {
    SoundJobSpec *spec;
    int result;

    if (D_004470E2 != 0) {
        if (func_001CBAC0() != 0) {
            return -1;
        }
        D_004470E2 = 0;
    }
    spec = job->spec;
    result = func_001CD020(job->unk18, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->unk1C);
    job->result = result;
    if (result >= 0) {
        D_00446868 += 1;
    }
    return 0;
}
