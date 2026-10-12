#include "kf_texture.h"
#include <eeregs.h>
#include <libgraph.h>

extern char D_004414A0[];           /* "load_tm2() tbp:0 !!!!" */

int puts(char *);
int FlushCache(int);

/* load_tm2(): sends a texture's image to GS memory, and its CLUT when the pixel format has one. */
void func_0020D0E8(TextureHeader *header) {
    sceGsLoadImage load_image;
    int pixel_format;
    u16 buffer_width;
    u16 image_format;
    s16 width;
    s16 height;

    if ((int)(header->tex0 & 0x3FFF) == 0) {
        puts(D_004414A0);
        return;
    }
    image_format = (header->tex0 >> 20) & 0x3F;
    buffer_width = (header->tex0 >> 14) & 0x3F;
    width = 1 << (int)((header->tex0 >> 26) & 0xF);
    height = 1 << (int)((header->tex0 >> 30) & 0xF);
    sceGsSetDefLoadImage(&load_image, (u16)(header->tex0 & 0x3FFF), buffer_width, image_format, 0, 0, width, height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + sizeof(TextureHeader));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    pixel_format = (header->tex0 >> 20) & 0x3F;
    if (pixel_format == 0x13 || pixel_format == 0x1B) {
        sceGsSetDefLoadImage(&load_image, (u16)((header->tex0 >> 37) & 0x3FFF), 1, (u16)((header->tex0 >> 51) & 0xF), 0, 0, 16, 16);
        FlushCache(0);
        sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
    } else if (pixel_format == 0x14 || pixel_format == 0x24 || pixel_format == 0x2C) {
        sceGsSetDefLoadImage(&load_image, (u16)((header->tex0 >> 37) & 0x3FFF), 1, (u16)((header->tex0 >> 51) & 0xF), 0, 0, 8, 2);
        FlushCache(0);
        sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
    }
}
