#include "common.h"

extern s32 D_0044734C;

/* Bump-allocates size bytes from the heap top D_0044734C, leaving the top 16-byte aligned. */
void *func_001FBD90(int size) {
    void *block = (void *)D_0044734C;
    u8 *top;
    s32 misalign;

    D_0044734C += size;
    // match: a pointer top keeps gcc from folding "- (misalign - 16)" into "+ 16 - misalign"
    top = (u8 *)D_0044734C;
    misalign = (s32)top & 0xF;
    if (misalign) {
        top -= misalign - 16;
    }
    D_0044734C = (s32)top;
    return block;
}
