#include "common.h"

/* Fills count words at dest with value. */
void func_002108F8(void *dest, int count, int value) {
    int *word = dest;

    while (count-- != 0) {
        *word++ = value;
    }
}
