#include "common.h"
#include "kf_sound.h"

int sceSifFreeIopHeap(int);
int puts(char *);
int FlushCache(int);
extern SoundState *D_004471D4;
extern char D_004469F0[];

/* Release the IOP heap block of a voice; returns -1 if none was held or the free failed. */
int func_001CD7E0(int voice_index) {
    SoundVoice *voice;

    voice = &D_004471D4->voices[voice_index];
    if (voice->iop_heap == 0) {
        return -1;
    }
    if (sceSifFreeIopHeap(voice->iop_heap) != 0) {
        return -1;
    }
    puts(D_004469F0);
    voice->iop_heap = 0;
    FlushCache(0);
    return 0;
}
