#include "common.h"

int func_0021F1F0(int, u32, u32, u32, u32, u32, u32, u32, u32, u32);
int func_0021F278(s64, u32, u32);
int func_0021F2F8(s64, u32, u32, u32);

/* Sends the corner points of two rects (x, y, width, height). */
int func_0021EE78(int reserved, s32 *rect_a, s32 *rect_b) {
    return func_0021F2F8(func_0021F278(func_0021F2F8(func_0021F278(func_0021F1F0(reserved, 6, 0, 1, 0, 0, 0, 1, 0, 0), rect_b[0], rect_b[1]), rect_a[0], rect_a[1], 0), rect_b[0] + rect_b[2], rect_b[1] + rect_b[3]), rect_a[0] + rect_a[2], rect_a[1] + rect_a[3], 0);
}
