#include "common.h"

extern s8 D_00446C09;

/* Moves value back and forth between low and high, one sixtieth of the span per call; D_00446C09 is the direction. */
f32 func_001E6A20(s32 low, s32 high, f32 value) {
    if (D_00446C09 == 0) {
        value += (f32)(high - low) / 60.0f;
        if (value > high) {
            value = high;
            D_00446C09 = 1;
        }
    } else {
        value -= (f32)(high - low) / 60.0f;
        if (value < low) {
            D_00446C09 = 0;
            value = low;
        }
    }
    return value;
}
