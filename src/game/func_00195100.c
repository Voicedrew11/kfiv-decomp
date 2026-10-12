#include "common.h"

/* Moves *state toward target over frames steps; with frames <= 1 it snaps. Returns the remaining gap. */
f32 func_00195100(f32 target, f32 frames, f32 *state) {
    f32 old;
    f32 gap;

    if (frames <= 1.0f) {
        *state = target;
        return 0.0f;
    }
    old = *state;
    gap = target - old;
    *state = old + gap / frames;
    return gap;
}
