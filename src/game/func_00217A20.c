#include "kf_game.h"
#include <eeregs.h>
#include <libdma.h>
#include <libgraph.h>

extern TextLog D_01E0E9C0;
extern u32 D_01E0E940[8];
extern char D_004337E0[0x40];
extern char D_00433820[];

int FlushCache(int);

/* Loads the text log's font (D_00433820, 256 by 128 at 4 bits) and its 16-colour palette (D_004337E0) into
   GS memory at the given pages, then tells the GS to flush its texture cache. */
void func_00217A20(int font_base, int palette_base) {
    sceGsLoadImage image;
    u32 *packet;

    sceDmaGetChan(2);
    D_01E0E9C0.font_base = font_base;
    D_01E0E9C0.font_buffer_width = 4;
    D_01E0E9C0.palette_base = palette_base;
    sceGsSetDefLoadImage(&image, D_01E0E9C0.font_base, D_01E0E9C0.font_buffer_width, 0x14, 0, 0, 0x100, 0x80);
    FlushCache(0);
    sceGsExecLoadImage(&image, D_00433820);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    sceGsSetDefLoadImage(&image, D_01E0E9C0.palette_base, 1, 0, 0, 0, 8, 2);
    FlushCache(0);
    sceGsExecLoadImage(&image, D_004337E0);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    packet = (u32 *)((u32)D_01E0E940 | 0x20000000);
    *packet++ = 0x8001;
    *packet++ = 0x10004000;
    *packet++ = 0xE;
    *packet++ = 0;
    *packet++ = 0xFFFFFFFF;
    *packet++ = 0;
    *packet++ = 3;
    *packet = 0;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = 2;
    *D2_MADR = (int)D_01E0E940;
    *D2_CHCR = 0x100;
}
