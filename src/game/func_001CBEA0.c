#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Turns the sound output off if it is on. */
void func_001CBEA0(void) {
    if (D_004471D4->active != FALSE) {
        func_001CBAE0(1, 0x50);
        D_004471D4->active = FALSE;
    }
}
