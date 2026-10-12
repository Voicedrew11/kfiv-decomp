#include "kf_splash.h"
#include "kf_texture.h"
#include <eeregs.h>
#include <libgraph.h>

#define HEADER_TEX0(header) ((BootTex0 *)&(header)->tex0)

extern u64 D_00420DC0[];

int FlushCache(int);

/* Sends a texture's image to GS memory a strip of rows at a time, and then its CLUT when the pixel format has one. */
void func_00204580(TextureHeader *header) {
    sceGsLoadImage load_image;
    u16 width = 1 << HEADER_TEX0(header)->tw;
    u16 height = 1 << HEADER_TEX0(header)->th;
    u16 base_pointer = HEADER_TEX0(header)->tbp0;
    char *image;
    f32 strip_rows;
    f32 rows;
    int pixel_format;
    int clut_width;
    int clut_height;

    // match: D_00420DC0 is a table of pointers; its address is never NULL and the store overwrites two of them
    if (D_00420DC0 != NULL) {
        *D_00420DC0 = header->tex0;
    }
    image = (char *)header + sizeof(TextureHeader);
    strip_rows = 131068.0f / (((f32)(u32)header->data_offset - 16.0f) / width);
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
    clut_width = (pixel_format & 4) ? 8 : 16;
    clut_height = (pixel_format & 4) ? 2 : 16;
    sceGsSetDefLoadImage(&load_image, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
}
