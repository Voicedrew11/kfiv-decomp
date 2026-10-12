#include "kf_sound.h"

extern SoundState *D_004471D4;

/* Sets where in sound memory a voice's data goes. */
int func_001CDB58(u32 voice_index, int address) {
    if (voice_index >= 16) {
        return -1;
    }
    D_004471D4->voices[voice_index].unk0C = address;
    return 0;
}
