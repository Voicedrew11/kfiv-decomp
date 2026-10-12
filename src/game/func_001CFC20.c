#include "common.h"

extern int D_004471EC;
extern u8 D_005C5A18[];

/* Starts the next 0x108-byte debug log slot with one character. */
void func_001CFC20(char first_char) {
    D_005C5A18[D_004471EC * 0x108] = first_char;
    D_004471EC++;
}
