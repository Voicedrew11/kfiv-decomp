/* The area the game loads its files into, which is also its sound heap. */
#include "common.h"

/* 0x249680 the file-load buffer; the sound heap starts at its 64-byte boundary */
char D_00249680[0x100000] = { 0 };

/* D_002496BF names the heap's base byte: ((u32)D_002496BF >> 6) << 6 is D_00249680 */
asm(".globl D_002496BF\nD_002496BF = D_00249680 + 0x3F\n");
