#include "kf_sound.h"

extern SoundState *D_004471D4;

/* The index of the first free voice, or -1 when all 16 are taken. */
int func_001CC160(void) {
    SoundVoice *voice = D_004471D4->voices;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
            break;
        }
    }
    if (!(i < 16)) {
        return -1;
    }
    return i;
}
