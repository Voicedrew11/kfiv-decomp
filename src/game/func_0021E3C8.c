#include "common.h"

void *memcpy(void *, const void *, u32);

/* Copies a source in two stretches into a destination in two stretches; returns the bytes copied, 0 if they do not fit. */
int func_0021E3C8(u8 *dest0, s32 dest0_size, u8 *dest1, s32 dest1_size, u8 *source0, s32 source0_size, u8 *source1,
                  s32 source1_size) {
    if (dest0_size + dest1_size < source0_size + source1_size) {
        return 0;
    }
    if (source0_size >= dest0_size) {
        memcpy(dest0, source0, dest0_size);
        memcpy(dest1, source0 + dest0_size, source0_size - dest0_size);
        memcpy(dest1 + source0_size - dest0_size, source1, source1_size);
    } else if (source1_size >= dest0_size - source0_size) {
        memcpy(dest0, source0, source0_size);
        memcpy(dest0 + source0_size, source1, dest0_size - source0_size);
        memcpy(dest1, source1 + dest0_size - source0_size, source1_size - (dest0_size - source0_size));
    } else {
        memcpy(dest0, source0, source0_size);
        memcpy(dest0 + source0_size, source1, source1_size);
    }
    return source0_size + source1_size;
}
