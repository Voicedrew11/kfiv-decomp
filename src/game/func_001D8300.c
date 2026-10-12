#include "common.h"

extern s8 D_00446C09;

/* Steps value toward `target` by (target - start) / 60 per call, bouncing between the two ends. */
f32 func_001D8300(f32 value, s32 start, s32 target) {
    f32 bound;

    if (D_00446C09 == 0) {
        bound = (f32) target;
        value = value + ((f32) (target - start) / 60.0f);
        if (bound < value) {
            value = bound;
            D_00446C09 = 1;
        }
    } else {
        bound = (f32) start;
        value = value - ((f32) (target - start) / 60.0f);
        if (value < bound) {
            D_00446C09 = 0;
            value = bound;
        }
    }
    return value;
}
