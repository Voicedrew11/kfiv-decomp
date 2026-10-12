#include "common.h"
#include "kf_sound.h"

extern SoundJob D_004FF4D8;
extern SoundStartSpec D_004FF4C0;

int func_001A0BD0(void *); // match: this file only matches with this type; func_001A0BD0.c defines it differently

/* Runs the step 2 sound load into bank index; returns 0 and the voice once it is done. */
int func_001A0C18(int args_addr, int index, int voice_out) {
    SoundStartArgs *args = (SoundStartArgs *)args_addr;
    int pending = 1;

    D_004FF4D8.unk18 = index + 6;
    D_004FF4D8.unk1C = index * 0x11800 + 0x138810;
    D_004FF4C0.buffer_a = (void *)args->unk18;
    D_004FF4C0.buffer_b = (void *)args->unk14;
    D_004FF4C0.size_a = args->unk1C;
    D_004FF4C0.size_b = args->unk00;
    D_004FF4D8.spec = (SoundJobSpec *)&D_004FF4C0;
    D_004FF4D8.result = index + 6;
    if (((int (*)(void *, int))func_001A0BD0)(&D_004FF4D8, index) == 0) { // match: a second argument the callee does not take
        *(u8 *)voice_out = D_004FF4D8.result;
        pending = 0;
    }
    return pending;
}
