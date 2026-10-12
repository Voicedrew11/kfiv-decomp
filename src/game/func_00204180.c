#include "kf_load.h"
#include "kf_splash.h"
#include "kf_texture.h"
#include <eeregs.h>
#include <libgraph.h>

#define HEADER_TEX0(header) ((BootTex0 *)&(header)->tex0)

int FlushCache(int);

/* Sends a texture's image to GS memory, in two halves when it is large, and then its CLUT when the pixel format has one. */
void func_00204180(TextureHeader *header, LoadedBlockRef *block) {
    sceGsLoadImage clut_load;
    sceGsLoadImage image_load;
    u64 buffer_width;
    u32 width;
    u32 height;
    u32 pixel_size;
    int pixel_format;
    int clut_width;
    int clut_height;

    buffer_width = 1 << HEADER_TEX0(header)->tw;
    HEADER_TEX0(header)->tbw = buffer_width >> 6;
    width = 1 << HEADER_TEX0(header)->tw;
    height = 1 << HEADER_TEX0(header)->th;
    pixel_format = HEADER_TEX0(header)->psm;
    if (pixel_format == 0x13 || pixel_format == 0x1B) {
        pixel_size = 2;
    } else if (pixel_format == 0x14 || pixel_format == 0x2C || pixel_format == 0x24) {
        pixel_size = 1;
    } else {
        pixel_size = 8;
    }
    if (width * height * pixel_size > 0x7FFFF) {
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height >> 1);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + sizeof(TextureHeader));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, height >> 1, width, height >> 1);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + ((width * height * pixel_size >> 2) + sizeof(TextureHeader)));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
    } else {
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + sizeof(TextureHeader));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
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
    sceGsSetDefLoadImage(&clut_load, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&clut_load, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
}
