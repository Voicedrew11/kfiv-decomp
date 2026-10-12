#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Turns the sound output on, once. */
void func_001CBE60(void) {
    if (D_004471D4->active != TRUE) {
        func_001CBAE0(1, 0x40);
        D_004471D4->active = TRUE;
    }
}
