#include "kf_debug.h"
#include "kf_game.h"
#include "kf_pad.h"
#include "kf_message.h"

static PadReport *debug_pad; /* 0x447108 the controller report the editors read; nothing in the retail game sets it */

int sprintf(char *, const char *, ...);
extern char D_004468F0[];
extern char D_004468F8[];
static char D_005742E0[0x68]; /* 0x5742E0 the strings the debug menus draw, four bytes apart */

void func_001B0A58(void);
void func_001B0A60(void);
void func_001B0A68(void);
void func_001B0A70(void);
int func_001B0A78(void);
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010A4E8(int, int, int, int, int);
void func_001B5460(void);
void Display_SwapBuffers(int);
extern DebugStartMenuSpacer D_0043EF70;
extern DebugStartMenuPrompt D_0043EF90;
extern DebugStartMenuItems D_0043EFD8;
extern u16 D_00413DA8[];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern char D_0043F1D8[];
extern char D_00446900[];
extern char D_00446908[];
extern char D_00446910[];

void func_001B02D8(int *cursor, int count) {
    int i;

    if (*cursor < 0) {
        *cursor = count - 1;
    }
    if (*cursor >= count) {
        *cursor = 0;
    }
    for (i = 0; i < count; i++) {
        if (i == *cursor) {
            sprintf(&D_005742E0[i * 4], D_004468F0);
        } else {
            sprintf(&D_005742E0[i * 4], D_004468F8);
        }
    }
}

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

f32 func_001B0950(f32 fparg0) {
    if (fparg0 < 0.0f) {
        return -fparg0;
    }
    return fparg0;
}

/* Dot product of two 3D vectors. */
float func_001B0970(void *left, void *right) {
    f32 *left_xyz = left;
    f32 *right_xyz = right;

    return left_xyz[0] * right_xyz[0] + left_xyz[1] * right_xyz[1] + left_xyz[2] * right_xyz[2];
}

/* Wraps an angle in radians into [-pi, pi]. */
float func_001B09A0(float angle) {
    float wrapped;

    wrapped = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}

/* Debug menu loop: runs the menu until it returns zero. */
int func_001B0A00(void) {
    func_001B0A58();
    while (TRUE) {
        func_001B0A60();
        func_001B0A68();
        if (func_001B0A78() == 0) {
            return 0;
        }
        func_001B0A70();
        if (func_001B0A78() == 0) {
            return 0;
        }
    }
}

void func_001B0A58(void) {
}

void func_001B0A60(void) {
}

void func_001B0A68(void) {
}

void func_001B0A70(void) {
}

/* Debug start menu: waits on a PUSH START screen, then lets up and down pick one of four
   lines. Returns the line chosen, or -1. */
int func_001B0A78(void) {
    DebugStartMenuSpacer spacer = D_0043EF70;
    DebugStartMenuPrompt prompt = D_0043EF90;
    DebugStartMenuItems items = D_0043EFD8;
    int chosen = -1;
    int running = TRUE;
    int selected;
    int i;

    do {
        while (running) {
            func_001B5460();
            if (D_00413DA8[1] & 8) {
                break;
            }
            func_001095E0();
            func_001095D0();
            func_0010A4E8(0, 0, 0, 0, 0x2A);
            Message_DebugSetColor(0);
            Message_DebugPrintf(spacer.text);
            Message_DebugPrintf(prompt.text);
            Message_Update();
            func_001095D8();
            func_001096E8();
            func_00109800();
            Display_SwapBuffers(0);
        }
        selected = 0;
        while (running) {
            func_001B5460();
            if (D_00413DA8[1] & 0x10) {
                selected = (selected + 3) % 4;
            } else if (D_00413DA8[1] & 0x40) {
                selected = (selected + 1) % 4;
            } else if (D_00413DA8[1] & 0x2000) {
                break;
            } else if (D_00413DA8[1] & 0x4000) {
                chosen = selected;
                running = FALSE;
            }
            func_001095E0();
            func_001095D0();
            func_0010A4E8(0, 0, 0, 0, 0x2A);
            Message_DebugPrintf(spacer.text);
            for (i = 0; i < 4; i++) {
                Message_DebugSetColor((selected == i) ? 3 : 0);
                Message_DebugPrintf(items.text[i]);
            }
            Message_Update();
            func_001095D8();
            func_001096E8();
            func_00109800();
            Display_SwapBuffers(0);
        }
    } while (running);
    return chosen;
}

/* Debug map picker: left and right (or L2 and R2 held) step *map_id through 0..254 on a
   "MAP nnn" screen. Returns TRUE if the choice was confirmed, FALSE if cancelled. */
int func_001B0E50(int *map_id) {
    s64 saved_colours[4];
    int confirmed = FALSE;
    int map = *map_id;
    PadReport *report;
    u16 pressed;
    u16 held;

    saved_colours[0] = D_00432E20[0];
    saved_colours[1] = D_00432E20[30];
    saved_colours[2] = D_00433050[0];
    saved_colours[3] = D_00433050[30];
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
        pressed = report->pressed;
        held = report->held;
        if (pressed & 0x80) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (pressed & 0x20) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        } else if (held & 0x400) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (held & 0x800) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        }
        func_001095E0();
        func_001095D0();
        Message_DebugPrintf(D_0043F1D8, map);
        Message_Update();
        func_001095D8();
        func_001096E8();
        ((int (*)(void))func_00109800)(); // match: a result keeps $2 busy after the call
        if (pressed & 0x2000) {
            *map_id = map;
            confirmed = TRUE;
            break;
        }
    } while (!(pressed & 0x4000));
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
    } while (report->held != 0);
    D_00432E20[0] = saved_colours[0];
    D_00432E20[30] = saved_colours[1];
    D_00433050[0] = saved_colours[2];
    D_00433050[30] = saved_colours[3];
    return confirmed;
}

void func_001B1028(void) {
}

/* Debug editor for a four-digit decimal number on an "INI:" screen: left and right move
   the cursor, up and down change the digit under it. Stores the number on confirm. */
void func_001B1030(int *value) {
    s64 saved_colours[4];
    int digits[4];
    int cursor = 0;
    int i;
    PadReport *report;
    u16 pressed;

    digits[0] = *value % 10;
    digits[1] = *value / 10 % 10;
    digits[2] = *value / 100 % 10;
    digits[3] = *value / 1000 % 10;
    saved_colours[0] = D_00432E20[0];
    saved_colours[1] = D_00432E20[30];
    saved_colours[2] = D_00433050[0];
    saved_colours[3] = D_00433050[30];
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
        pressed = report->pressed;
        if ((pressed & 0x80) && cursor < 3) {
            cursor++;
        } else if ((pressed & 0x20) && cursor > 0) {
            cursor--;
        } else if (pressed & 0x10) {
            digits[cursor] = (digits[cursor] + 1) % 10;
        } else if (pressed & 0x40) {
            digits[cursor] = (digits[cursor] + 9) % 10;
        }
        func_001095E0();
        func_001095D0();
        Message_DebugSetColor(9);
        Message_DebugPrintf(D_00446900);
        for (i = 3; i >= 0; i--) {
            Message_DebugSetColor((cursor == i) ? 8 : 9);
            Message_DebugPrintf(D_00446908, digits[i]);
        }
        Message_DebugPrintf(D_00446910);
        Message_Update();
        func_001095D8();
        func_001096E8();
        ((int (*)(void))func_00109800)(); // match: a result keeps $2 busy after the call
        if (pressed & 0x2000) {
            *value = digits[0] + digits[1] * 10 + digits[2] * 100 + digits[3] * 1000;
            break;
        }
    } while (!(pressed & 0x4000));
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
    } while (report->held != 0);
    D_00432E20[0] = saved_colours[0];
    D_00432E20[30] = saved_colours[1];
    D_00433050[0] = saved_colours[2];
    D_00433050[30] = saved_colours[3];
}
