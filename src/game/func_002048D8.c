#include "kf_splash.h"
#include "kf_texture.h"
#include <eeregs.h>
#include <libgraph.h>

#define HEADER_TEX0(header) ((BootTex0 *)&(header)->tex0)

int FlushCache(int);

/* Sends a texture's image to GS memory a strip of rows at a time, then its CLUT and a second CLUT from the file; hands back
   the TEX0 word and the word after the first CLUT, which is the second CLUT's TEX0. */
void func_002048D8(TextureHeader *header, u64 *tex0_out, u64 *trailer_out) {
    sceGsLoadImage load_image;
    sceGsLoadImage second_load;
    u16 width = 1 << HEADER_TEX0(header)->tw;
    u16 height = 1 << HEADER_TEX0(header)->th;
    u16 base_pointer = HEADER_TEX0(header)->tbp0;
    char *image;
    f32 strip_rows;
    f32 rows;
    int pixel_format;
    u16 clut_width;
    u16 clut_height;
    f32 pixel_bytes;
    u64 *trailer;

    if (tex0_out != NULL) {
        *tex0_out = header->tex0;
    }
    // match: no code; without it gcc loads TEX0 into other registers ($7/$5 in retail), same instructions otherwise
    __asm__ volatile("" : : : "memory");
    image = (char *)header + sizeof(TextureHeader);
    switch (HEADER_TEX0(header)->psm) {
    case 0x00:
    case 0x30:
        pixel_bytes = 4.0f;
        break;
    case 0x01:
    case 0x31:
        pixel_bytes = 3.0f;
        break;
    case 0x02:
    case 0x0A:
    case 0x32:
    case 0x3A:
        pixel_bytes = 2.0f;
        break;
    case 0x13:
    case 0x1B:
        pixel_bytes = 1.0f;
        break;
    default:
        pixel_bytes = 0.5f;
        break;
    }
    pixel_bytes *= width * height;
    strip_rows = 131068.0f / ((pixel_bytes - 16.0f) / width);
    strip_rows = (int)(strip_rows * 0.015625f) << 6;
    rows = (height < strip_rows) ? height : strip_rows;
    strip_rows = rows;
    while (height != 0) {
        if (rows < height) {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, (int)strip_rows);
            FlushCache(0);
            while (sceGsExecLoadImage(&load_image, image) != 0) {
            }
            height -= (int)strip_rows;
            base_pointer += HEADER_TEX0(header)->tbw * (int)strip_rows;
            image += (HEADER_TEX0(header)->tbw * (int)strip_rows) << 8;
        } else {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height);
            FlushCache(0);
            sceGsExecLoadImage(&load_image, image);
            height = 0;
        }
    }
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    pixel_format = HEADER_TEX0(header)->psm;
    if ((pixel_format >> 4) == 0 || (pixel_format >> 4) == 3) {
        return;
    }
    clut_width = 16;
    if (pixel_format & 4) {
        clut_width = 8;
    }
    clut_height = 16;
    if (pixel_format & 4) {
        clut_height = 2;
    }
    sceGsSetDefLoadImage(&load_image, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    trailer = (u64 *)((char *)header + clut_width * clut_height * 4 + (header->image_size + sizeof(TextureHeader)));
    if (trailer_out != NULL) {
        *trailer_out = *trailer;
    }
    sceGsSetDefLoadImage(&second_load, ((BootTex0 *)trailer)->cbp, 1, ((BootTex0 *)trailer)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&second_load, (char *)header + header->unk0C);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
}
