#include "kf_game.h"
#include <eeregs.h>

extern TextLog D_01E0E9C0;
extern int D_01E0E960[0x18];

/* Sends the GS registers the text log draws with: its font texture (256 by 128, 4-bit, palette in the texture
   page), point sampling, alpha blending. Each register is a 64-bit value followed by its address. */
void func_00217DA0(void) {
    int *packet;

    packet = (int *)((u32)D_01E0E960 | 0x20000000);
    *packet++ = 0x8005;
    *packet++ = 0x10004000;
    *packet++ = 0xE;
    *packet++ = 0;
    /* TEX0: TBP0, TBW, PSM, TW, TH, TCC, TFX, CBP, CPSM, CSM, CLD */
    *(u64 *)packet = (u64)D_01E0E9C0.font_base | ((u64)D_01E0E9C0.font_buffer_width << 14) | ((u64)0x14 << 20) |
                     ((u64)8 << 26) | ((u64)7 << 30) | ((u64)1 << 34) | ((u64)0 << 35) |
                     ((u64)D_01E0E9C0.palette_base << 37) | ((u64)0 << 51) | ((u64)0 << 55) | ((u64)4 << 61);
    packet += 2;
    *(u64 *)packet = 6;
    packet += 2;
    *(u64 *)packet = 0x60;
    packet += 2;
    *(u64 *)packet = 0x14;
    packet += 2;
    *(u64 *)packet = 0;
    packet += 2;
    *(u64 *)packet = 0x49;
    packet += 2;
    *(u64 *)packet = 0x44;
    packet += 2;
    *(u64 *)packet = 0x42;
    packet += 2;
    *(u64 *)packet = 1;
    ((u64 *)packet)[1] = 0x1C;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = 6;
    *D2_MADR = (int)D_01E0E960;
    *D2_CHCR = 0x100;
}
