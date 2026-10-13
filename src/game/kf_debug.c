#include "kf_debug.h"
#include "kf_game.h"
#include "kf_pad.h"

static PadReport *debug_pad; /* 0x447108 the controller report the editors read; nothing in the retail game sets it */

static inline int debug_pad_step(int mask) {
    return debug_pad->pressed & mask;
}

static inline int debug_pad_modifier(int mask) {
    return debug_pad->held & mask;
}

/* Debug editor for an int: left and right change *value by step (times 10, 100 or 1000
   with the modifier buttons), clamped to [minimum, maximum]. */
void Debug_EditInt(s32 *value, s32 minimum, s32 maximum, s32 step) {
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

/* Debug_EditInt for a u16. */
void Debug_EditU16(u16 *value, u16 minimum, u16 maximum, u16 step) {
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

/* Debug_EditInt for a byte. */
void Debug_EditU8(u8 *value, u8 minimum, u8 maximum, u8 step) {
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

/* Debug_EditInt for a float. */
void Debug_EditFloat(f32 *value, f32 minimum, f32 maximum, f32 step) {
    if (debug_pad_step(0x80) && debug_pad_modifier(0x3000) == 0x3000) {
        *value -= step * 1000.0f;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x3000) == 0x3000) {
        *value += step * 1000.0f;
    } else if (debug_pad_step(0x80) && debug_pad_modifier(0x1000)) {
        *value -= step * 100.0f;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x1000)) {
        *value += step * 100.0f;
    } else if (debug_pad_step(0x80) && debug_pad_modifier(0x2000)) {
        *value -= step * 10.0f;
    } else if (debug_pad_step(0x20) && debug_pad_modifier(0x2000)) {
        *value += step * 10.0f;
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
