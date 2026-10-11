#include "common.h"
#include "kf_screen.h"

extern char D_00471DE0[];

float sinf(float);

/* Advances each of the 16 entries' phase and recomputes its sine offsets. */
void func_00111D58(void) {
    WaveOscillator *entry = (WaveOscillator *)D_00471DE0;
    float step = 0.06981317f;
    float half_turn = 3.1415927f;
    float two_pi = 6.2831855f;
    int i;

    for (i = 0; i < 16; i++) {
        entry->phase += step;
        if (half_turn <= entry->phase) {
            entry->phase -= two_pi;
        }
        entry->x = entry->amp_x * sinf(entry->phase) * 4096.0f;
        entry->y = entry->amp_y * sinf(entry->phase) * 4096.0f;
        entry++;
    }
}
