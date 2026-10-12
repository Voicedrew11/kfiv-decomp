#include "kf_game.h"

/* the controller report the debug value editors read; nothing in the retail game sets it */
extern PadReport *D_00447108;

static inline int debug_pad_step(int mask) {
    return D_00447108->pressed & mask;
}

static inline int debug_pad_modifier(int mask) {
    return D_00447108->held & mask;
}

/* Debug editor for an int: left and right change *value by step (times 10, 100 or 1000
   with the modifier buttons), clamped to [minimum, maximum]. */
void func_001B0390(s32 *value, s32 minimum, s32 maximum, s32 step) {
    if (debug_pad_step(0x80) && debug_pad_modifier(0x3000) == 0x3000) {
        *value -= step * 1000;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x3000) == 0x3000) {
        *value += step * 1000;
    } else if (debug_pad_step(0x80) && debug_pad_modifier(0x1000)) {
        *value -= step * 100;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x1000)) {
        *value += step * 100;
    } else if (debug_pad_step(0x80) && debug_pad_modifier(0x2000)) {
        *value -= step * 10;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x2000)) {
        *value += step * 10;
    } else if (debug_pad_step(0x80)) {
        *value -= step;
    } else if (debug_pad_step(0x20)) {
        *value += step;
    }
    if (*value < minimum) {
        *value = minimum;
    }
    if (*value > maximum) {
        *value = maximum;
    }
}
