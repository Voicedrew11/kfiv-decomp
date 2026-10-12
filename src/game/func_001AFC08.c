#include "common.h"
#include "kf_effect.h"

/* Progress from min to max, eased by mode (1: squared, 2: ease out), written as 1 - progress. */
void func_001AFC08(f32 *result, f32 *eased, int value, int min, int max, int mode) {
    f32 fraction = ((f32)value - (f32)min) / ((f32)max - (f32)min);
    *eased = fraction;
    if (mode == 1) {
        *eased = fraction * fraction;
    } else if (mode == 2) {
        f32 remaining = 1.0f - fraction;
        *eased = 1.0f - remaining * remaining;
    }
    *result = 1.0f - *eased;
}
