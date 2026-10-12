#include "kf_sound.h"

int sceSifFreeIopHeap(int);
int FlushCache(int);
extern SoundState *D_004471D4;

/* Frees a voice's two data blocks on the IOP; -1 if either was missing or would not free. */
int func_001CD840(int voice_index) {
    SoundVoice *voice = &D_004471D4->voices[voice_index];
    int result = 0;

    if (voice->unk00 != 0) {
        if (sceSifFreeIopHeap(voice->unk00) != 0) {
            result = -1;
        } else {
            voice->unk00 = 0;
        }
    } else {
        result = -1;
    }
    if (voice->unk04 != 0) {
        if (sceSifFreeIopHeap(voice->unk04) != 0) {
            result = -1;
        } else {
            voice->unk04 = 0;
        }
    } else {
        result = -1;
    }
    FlushCache(0);
    return result;
}
