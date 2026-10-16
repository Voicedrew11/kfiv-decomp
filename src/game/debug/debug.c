#include "debug/debug.h"
#include "core/game.h"
#include "input/pad.h"
#include "ui/message.h"

static PadReport *debug_pad; /* 0x447108 the controller report the editors read; nothing in the retail game sets it */

int sprintf(char *, const char *, ...);
extern char debug_cursor_mark[];
extern char debug_cursor_blank[];
static char debug_cursor_marks[0x68]; /* 0x5742E0 the strings the debug menus draw, four bytes apart */

void func_001B0A58(void);
void func_001B0A60(void);
void func_001B0A68(void);
void func_001B0A70(void);
int Debug_RunStartMenu(void);
void func_001095D0(void);
void func_001095D8(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void GsDraw_FillScreen(int, int, int, int, int);
void Input_Update(void);
void Display_SwapBuffers(int);
extern DebugStartMenuSpacer debug_start_menu_spacer;
extern DebugStartMenuPrompt debug_start_menu_prompt;
extern DebugStartMenuItems debug_start_menu_items;
extern u16 pad_state[];
extern s64 game_dbuff_clear_color[];
extern s64 game_dbuff_context2_clear_color[];
extern char debug_map_format[];
extern char debug_ini_prompt[];
extern char debug_ini_digit_format[];
extern char debug_ini_line_end[];

/* Wraps the cursor into 0..count-1 and writes ">>" for its line and "  " for the others into debug_cursor_marks.
   Nothing calls it. */
void Debug_MarkCursorLine(int *cursor, int count) {
    int i;

    if (*cursor < 0) {
        *cursor = count - 1;
    }
    if (*cursor >= count) {
        *cursor = 0;
    }
    for (i = 0; i < count; i++) {
        if (i == *cursor) {
            sprintf(&debug_cursor_marks[i * 4], debug_cursor_mark);
        } else {
            sprintf(&debug_cursor_marks[i * 4], debug_cursor_blank);
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
    if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value -= step * 1000;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value += step * 1000;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value -= step * 100;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value += step * 100;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value -= step * 10;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value += step * 10;
    } else if (debug_pad_step(PAD_LEFT)) {
        *value -= step;
    } else if (debug_pad_step(PAD_RIGHT)) {
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
    if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value -= step * 1000;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value += step * 1000;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value -= step * 100;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value += step * 100;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value -= step * 10;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value += step * 10;
    } else if (debug_pad_step(PAD_LEFT)) {
        *value -= step;
    } else if (debug_pad_step(PAD_RIGHT)) {
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
    if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value -= step * 1000;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE | PAD_CIRCLE) == (PAD_TRIANGLE | PAD_CIRCLE)) {
        *value += step * 1000;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value -= step * 100;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_TRIANGLE)) {
        *value += step * 100;
    } else if (debug_pad_step(PAD_LEFT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value -= step * 10;
    } else if (debug_pad_step(PAD_RIGHT) && debug_pad_modifier(PAD_CIRCLE)) {
        *value += step * 10;
    } else if (debug_pad_step(PAD_LEFT)) {
        *value -= step;
    } else if (debug_pad_step(PAD_RIGHT)) {
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

/* The absolute value of a float. */
f32 Math_AbsFloat(f32 fparg0) {
    if (fparg0 < 0.0f) {
        return -fparg0;
    }
    return fparg0;
}

/* Dot product of two 3D vectors. */
float Vector_Dot3(void *left, void *right) {
    f32 *left_xyz = left;
    f32 *right_xyz = right;

    return left_xyz[0] * right_xyz[0] + left_xyz[1] * right_xyz[1] + left_xyz[2] * right_xyz[2];
}

/* Wraps an angle in radians into [-pi, pi], as Angle_Wrap does. Nothing calls it. */
float Angle_WrapPlusMinusPi(float angle) {
    float wrapped;

    wrapped = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}

/* Runs the debug start menu until its first line (NEW GAME) is chosen; returns 0. The four calls around it are
   empty. Nothing calls it. */
int Debug_LoopUntilNewGame(void) {
    func_001B0A58();
    while (TRUE) {
        func_001B0A60();
        func_001B0A68();
        if (Debug_RunStartMenu() == 0) {
            return 0;
        }
        func_001B0A70();
        if (Debug_RunStartMenu() == 0) {
            return 0;
        }
    }
}

/* Empty. */
void func_001B0A58(void) {
}

/* Empty. */
void func_001B0A60(void) {
}

/* Empty. */
void func_001B0A68(void) {
}

/* Empty. */
void func_001B0A70(void) {
}

/* Debug start menu: waits on a PUSH START screen, then lets up and down pick one of four
   lines (the first is NEW GAME). Returns the line chosen, or -1. */
int Debug_RunStartMenu(void) {
    DebugStartMenuSpacer spacer = debug_start_menu_spacer;
    DebugStartMenuPrompt prompt = debug_start_menu_prompt;
    DebugStartMenuItems items = debug_start_menu_items;
    int chosen = -1;
    int running = TRUE;
    int selected;
    int i;

    do {
        while (running) {
            Input_Update();
            if (pad_state[1] & PAD_START) {
                break;
            }
            Packet_SwapBuffers();
            func_001095D0();
            GsDraw_FillScreen(0, 0, 0, 0, 0x2A);
            Message_DebugSetColor(0);
            Message_DebugPrintf(spacer.text);
            Message_DebugPrintf(prompt.text);
            Message_Update();
            func_001095D8();
            Packet_SendChain();
            Packet_WaitForSend();
            Display_SwapBuffers(0);
        }
        selected = 0;
        while (running) {
            Input_Update();
            if (pad_state[1] & PAD_UP) {
                selected = (selected + 3) % 4;
            } else if (pad_state[1] & PAD_DOWN) {
                selected = (selected + 1) % 4;
            } else if (pad_state[1] & PAD_CIRCLE) {
                break;
            } else if (pad_state[1] & PAD_CROSS) {
                chosen = selected;
                running = FALSE;
            }
            Packet_SwapBuffers();
            func_001095D0();
            GsDraw_FillScreen(0, 0, 0, 0, 0x2A);
            Message_DebugPrintf(spacer.text);
            for (i = 0; i < 4; i++) {
                Message_DebugSetColor((selected == i) ? 3 : 0);
                Message_DebugPrintf(items.text[i]);
            }
            Message_Update();
            func_001095D8();
            Packet_SendChain();
            Packet_WaitForSend();
            Display_SwapBuffers(0);
        }
    } while (running);
    return chosen;
}

/* Debug map picker: left and right (or L1 and R1 held) step *map_id through 0..254 on a
   "MAP nnn" screen. Returns TRUE if the choice was confirmed, FALSE if cancelled. */
int Debug_PickMap(int *map_id) {
    s64 saved_colours[4];
    int confirmed = FALSE;
    int map = *map_id;
    PadReport *report;
    u16 pressed;
    u16 held;

    saved_colours[0] = game_dbuff_clear_color[0];
    saved_colours[1] = game_dbuff_clear_color[30];
    saved_colours[2] = game_dbuff_context2_clear_color[0];
    saved_colours[3] = game_dbuff_context2_clear_color[30];
    game_dbuff_clear_color[0] = 0xC000L << 17;
    game_dbuff_clear_color[30] = 0xC000L << 17;
    game_dbuff_context2_clear_color[0] = 0xC000L << 17;
    game_dbuff_context2_clear_color[30] = 0xC000L << 17;
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
        pressed = report->pressed;
        held = report->held;
        if (pressed & PAD_LEFT) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (pressed & PAD_RIGHT) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        } else if (held & PAD_L1) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (held & PAD_R1) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        }
        Packet_SwapBuffers();
        func_001095D0();
        Message_DebugPrintf(debug_map_format, map);
        Message_Update();
        func_001095D8();
        Packet_SendChain();
        ((int (*)(void))Packet_WaitForSend)(); // match: a result keeps $2 busy after the call
        if (pressed & PAD_CIRCLE) {
            *map_id = map;
            confirmed = TRUE;
            break;
        }
    } while (!(pressed & PAD_CROSS));
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
    } while (report->held != 0);
    game_dbuff_clear_color[0] = saved_colours[0];
    game_dbuff_clear_color[30] = saved_colours[1];
    game_dbuff_context2_clear_color[0] = saved_colours[2];
    game_dbuff_context2_clear_color[30] = saved_colours[3];
    return confirmed;
}

/* Empty. Nothing calls it. */
void func_001B1028(void) {
}

/* Debug editor for a four-digit decimal number on an "INI:" screen: left and right move
   the cursor, up and down change the digit under it. Stores the number on confirm. */
void Debug_EditIniNumber(int *value) {
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
    saved_colours[0] = game_dbuff_clear_color[0];
    saved_colours[1] = game_dbuff_clear_color[30];
    saved_colours[2] = game_dbuff_context2_clear_color[0];
    saved_colours[3] = game_dbuff_context2_clear_color[30];
    game_dbuff_clear_color[0] = 0xC000L << 17;
    game_dbuff_clear_color[30] = 0xC000L << 17;
    game_dbuff_context2_clear_color[0] = 0xC000L << 17;
    game_dbuff_context2_clear_color[30] = 0xC000L << 17;
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
        pressed = report->pressed;
        if ((pressed & PAD_LEFT) && cursor < 3) {
            cursor++;
        } else if ((pressed & PAD_RIGHT) && cursor > 0) {
            cursor--;
        } else if (pressed & PAD_UP) {
            digits[cursor] = (digits[cursor] + 1) % 10;
        } else if (pressed & PAD_DOWN) {
            digits[cursor] = (digits[cursor] + 9) % 10;
        }
        Packet_SwapBuffers();
        func_001095D0();
        Message_DebugSetColor(9);
        Message_DebugPrintf(debug_ini_prompt);
        for (i = 3; i >= 0; i--) {
            Message_DebugSetColor((cursor == i) ? 8 : 9);
            Message_DebugPrintf(debug_ini_digit_format, digits[i]);
        }
        Message_DebugPrintf(debug_ini_line_end);
        Message_Update();
        func_001095D8();
        Packet_SendChain();
        ((int (*)(void))Packet_WaitForSend)(); // match: a result keeps $2 busy after the call
        if (pressed & PAD_CIRCLE) {
            *value = digits[0] + digits[1] * 10 + digits[2] * 100 + digits[3] * 1000;
            break;
        }
    } while (!(pressed & PAD_CROSS));
    do {
        Display_SwapBuffers(0);
        Pad_Update();
        report = Pad_GetReport(0, 0);
    } while (report->held != 0);
    game_dbuff_clear_color[0] = saved_colours[0];
    game_dbuff_clear_color[30] = saved_colours[1];
    game_dbuff_context2_clear_color[0] = saved_colours[2];
    game_dbuff_context2_clear_color[30] = saved_colours[3];
}
