#include "common.h"

extern s32 D_005C58E8[];
extern s32 D_00420BEC[][4];

s32 func_001D0588(s16 *text, s32 slot);

/* Measures each line of a text stream and stores its centering offset. */
s16 *func_001D0658(s16 *text, s32 slot) {
    s32 line = 0;
    s32 *ctx = D_005C58E8;
    s32 *offsets = ctx + 14;
    s32 width = 0;

    s16 glyph;

    while (TRUE) {
        width += func_001D0588(text, slot);
        glyph = *text;
        if (glyph == -0x8000) {
            line++;
            *offsets++ = 0x140 - width / 2;
            width = 0;
            if (line >= D_00420BEC[slot][0]) {
                break;
            }
        }
        if (glyph == -1) {
            *offsets = 0x140 - width / 2;
            break;
        }
        text++;
    }
    return text;
}
