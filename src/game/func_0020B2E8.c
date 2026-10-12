#include "common.h"

/* Clears word_count 32-bit words. */
void func_0020B2E8(void *dest, u32 word_count) {
    s32 *words = dest;

    while (word_count--) {
        *words++ = 0;
    }
}
