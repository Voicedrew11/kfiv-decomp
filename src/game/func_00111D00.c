#include "kf_screen.h"

extern WaveOscillator D_00471DE0[];

/* Resets the 16 screen wobble oscillators, spreading their phases over one turn. */
void func_00111D00(void) {
    WaveOscillator *oscillator = D_00471DE0;
    int i;

    for (i = 0; i < 16; i++) {
        oscillator->amp_x = 0.01f;
        oscillator->amp_y = 0.0f;
        oscillator->phase = (float)i * 6.2831855f * 0.0625f;
        oscillator++;
    }
}
