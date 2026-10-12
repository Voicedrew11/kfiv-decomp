#include "kf_splash.h"

extern int D_0042C028[6];
extern u32 *D_00446E84;

void *memcpy(void *, const void *, u32);

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Builds, at D_00446E84, the packet that sends the splash image to GS memory at `tbp`, and its palette
   (when it has one) to `cbp`. */
void func_002087D0(int tbp, int cbp) {
    SplashImageBits *image;
    SplashLoadPacket *packet;
    char *source;
    char *output;
    u32 *cursor;
    int width;
    int height;
    int bits;
    int size;
    int qwords;
    int palette_qwords;
    int palette_width;
    int palette_height;
    int palette_buffer_width;
    int total;
    int has_palette;
    int psm;
    u64 format;

    image = (SplashImageBits *)&D_00249680;
    psm = image->tex0.psm;
    width = 1 << image->tex0.tw;
    height = 1 << image->tex0.th;
    bits = D_0042C028[psm & 7];
    size = width * height * bits / 8;
    qwords = (size + 15) / 16;
    if (bits == 4) {
        palette_qwords = 2;
        if (image->tex0.cpsm == 0) {
            palette_qwords = 4;
        }
        palette_width = 8;
        palette_height = 2;
    } else {
        palette_qwords = 0x20;
        if (image->tex0.cpsm == 0) {
            palette_qwords = 0x40;
        }
        palette_width = 16;
        palette_height = 16;
    }
    palette_buffer_width = 1;
    has_palette = bits <= 8;
    if (has_palette) {
        total = (qwords + 7) + (palette_qwords + 7);
    } else {
        total = qwords + 7;
    }

    /* the image: a DMA tag, then the same layout as SplashLoadPacket, written word by word */
    cursor = D_00446E84;
    *cursor++ = (total + 1) | 0x10000000;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = total | 0x51000000;
    *((u64 *)cursor)++ = 0x1000000000000005;
    *cursor++ = 0xE;
    *cursor++ = 0;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0x3F;
    *cursor++ = 0;
    format = image->tex0.psm;
    *((u64 *)cursor)++ = ((u64)tbp << 32) | ((u64)image->tex0.tbw << 48) | (format << 56);
    *cursor++ = 0x50;
    *cursor++ = 0;
    *((u64 *)cursor)++ = 0;
    *cursor++ = 0x51;
    *cursor++ = 0;
    *((u64 *)cursor)++ = width | ((u64)height << 32);
    *cursor++ = 0x52;
    *cursor++ = 0;
    *((u64 *)cursor)++ = 0;
    *cursor++ = 0x53;
    *cursor++ = 0;
    if (has_palette) {
        *((u64 *)cursor)++ = qwords | 0x800000000000000;
    } else {
        *((u64 *)cursor)++ = (qwords | 0x8000) | 0x800000000000000;
    }
    *cursor++ = 0;
    *cursor++ = 0;
    source = (char *)&D_00249680;
    source += sizeof(TextureHeader);
    memcpy(cursor, source, size);
    output = (char *)cursor + size;
    output = align16(output);

    if (has_palette) {
        packet = (SplashLoadPacket *)output;
        packet->tag[0] = 0x1000000000000005;
        packet->tag[1] = 0xE;
        packet->texflush[0] = 0xFFFFFFFF;
        packet->texflush[1] = 0;
        packet->texflush_reg[0] = 0x3F;
        packet->texflush_reg[1] = 0;
        format = image->tex0.cpsm;
        // match: the width has to be a variable, a literal 1 << 48 is combined with the base pointer first
        packet->bitbltbuf = ((u64)cbp << 32) | (((u64)palette_buffer_width << 48) | (format << 56));
        packet->bitbltbuf_reg[0] = 0x50;
        packet->bitbltbuf_reg[1] = 0;
        packet->trxpos = 0;
        packet->trxpos_reg[0] = 0x51;
        packet->trxpos_reg[1] = 0;
        packet->trxreg = palette_width | ((u64)palette_height << 32);
        packet->trxreg_reg[0] = 0x52;
        packet->trxreg_reg[1] = 0;
        packet->trxdir = 0;
        packet->trxdir_reg[0] = 0x53;
        packet->trxdir_reg[1] = 0;
        packet->image_tag = (palette_qwords | 0x8000) | 0x800000000000000;
        packet->image_tag_high[0] = 0;
        packet->image_tag_high[1] = 0;
        output += sizeof(SplashLoadPacket);
        source = (char *)&D_00249680;
        source += image->image_size + sizeof(TextureHeader);
        memcpy(output, source, palette_qwords << 4);
        // match: nothing reads this, but with no statement after it the call above becomes a tail jump
        output += palette_qwords << 4;
    }
}
