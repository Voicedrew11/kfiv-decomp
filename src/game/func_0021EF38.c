#include "common.h"

int func_0021F1F0(int, u32, u32, u32, u32, u32, u32, u32, u32, u32);
int func_0021F2A8(s64, u32, u32, u32, u32, u64);
int func_0021F2F8(int, u32, u32, u32);

/* Sends a colored primitive for the rect (x, y, width, height), color as RGBA bytes. */
int func_0021EF38(int reserved, s32 *rect, u32 color) {
    return func_0021F2F8(func_0021F2A8(func_0021F2F8(func_0021F1F0(reserved, 6, 0, 0, 0, 0, 0, 1, 0, 0), rect[0], rect[1], 0), color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, color >> 24, 0), rect[0] + rect[2], rect[1] + rect[3], 0);
}
