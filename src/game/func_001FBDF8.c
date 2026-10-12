#include "common.h"

extern s32 D_00446D38;
extern s32 D_00446D3C;

s32 func_001FBDF8(s32 size, s32 align) {
    s32 aligned;
    s32 end;

    aligned = ((D_00446D38 + align - 1) / align) * align;
    end = aligned + size;
    D_00446D38 = aligned;
    if (end - D_00446D3C > 0x280000) {
        return 0;
    }
    D_00446D38 = end;
    return aligned;
}
