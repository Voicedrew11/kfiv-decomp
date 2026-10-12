#include "common.h"

/* Clears byte_count bytes, eight at a time. */
void func_0020B278(char *dest, u32 byte_count) {
    int remaining = byte_count >> 3;

    while (remaining--) {
        *(s64 *)dest = 0;
        dest += 8;
    }
    remaining = byte_count & 7;
    while (remaining--) {
        *dest = 0; // match: retail does not step dest here, so only the first leftover byte is cleared
    }
}
