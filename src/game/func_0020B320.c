#include "common.h"

/* Copies word_count 32-bit words. */
void func_0020B320(s32 *dest, s32 *src, u32 word_count) {
    while (word_count--) {
        *dest++ = *src++;
    }
}
