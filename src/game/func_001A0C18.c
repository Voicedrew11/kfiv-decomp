#include "common.h"
#include "kf_sound.h"

extern SoundJob D_004FF4D8;
extern SoundStartSpec D_004FF4C0;

int func_001A0BD0(void *, int);

/* Runs the step 2 sound load into bank index; returns 0 and the voice once it is done. */
int func_001A0C18(int args_addr, int index, int voice_out) {
    SoundStartArgs *args = (SoundStartArgs *)args_addr;
    int pending = 1;

    D_004FF4D8.unk18 = index + 6;
    D_004FF4D8.unk1C = index * 0x11800 + 0x138810;
    D_004FF4C0.buffer_a = (void *)args->buffer_a;
    D_004FF4C0.buffer_b = (void *)args->buffer_b;
    D_004FF4C0.size_a = args->size_a;
    D_004FF4C0.size_b = args->size_b;
    D_004FF4D8.spec = (SoundJobSpec *)&D_004FF4C0;
    D_004FF4D8.result = index + 6;
    if (func_001A0BD0(&D_004FF4D8, index) == 0) {
        *(u8 *)voice_out = D_004FF4D8.result;
        pending = 0;
    }
    return pending;
}
