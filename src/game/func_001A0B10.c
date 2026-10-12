#include "common.h"
#include "kf_sound.h"

extern int D_00446864;
int func_001CD020(int, int, int, int, int, int, int);

/* Starts a sound job and counts it in D_00446864 when it was accepted. */
int func_001A0B10(SoundJob *job) {
    SoundJobSpec *spec = job->spec;
    int result = func_001CD020(job->unk18, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->unk1C);

    job->result = result;
    if (result >= 0) {
        D_00446864 += 1;
    }
    return 0;
}
