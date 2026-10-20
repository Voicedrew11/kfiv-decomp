#ifndef KF_DEBUG_H
#define KF_DEBUG_H

#include "common.h"
#include "core/game.h"

void Debug_EditInt(s32 *value, s32 minimum, s32 maximum, s32 step);
void Debug_EditU16(u16 *value, u16 minimum, u16 maximum, u16 step);
void Debug_EditU8(u8 *value, u8 minimum, u8 maximum, u8 step);
void Debug_EditFloat(f32 *value, f32 minimum, f32 maximum, f32 step);

/* debug/debug.c */
f32 Math_AbsFloat(f32 fparg0);
float Vector_Dot3(void *left, void *right);

/* data still in asm */
extern DebugStartMenuSpacer debug_start_menu_spacer;
extern DebugStartMenuPrompt debug_start_menu_prompt;
extern DebugStartMenuItems debug_start_menu_items;
extern char debug_map_format[];
extern char debug_cursor_mark[];
extern char debug_cursor_blank[];
extern char debug_ini_prompt[];
extern char debug_ini_digit_format[];
extern char debug_ini_line_end[];

#endif
