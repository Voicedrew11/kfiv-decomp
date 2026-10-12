#include "kf_texture.h"

extern char D_004414B8[];           /* "load_tm2()" */
extern char D_004414C8[];           /* "TBP[%u] " */
extern char D_004414D8[];           /* "TBW[%u] " */
extern char D_004414E8[];           /* "PSM[0x%x] " */
extern char D_00446FA8[];           /* "TW[%u] " */
extern char D_00446FB0[];           /* "TH[%u] " */
extern char D_004414F8[];           /* "TCC[%u] " */
extern char D_00441508[];           /* "TFX[0x%x] " */
extern char D_00441518[];           /* "CBP[%u] " */
extern char D_00441528[];           /* "CPSM[0x%x] " */
extern char D_00441538[];           /* "CSM[%u] " */
extern char D_00441548[];           /* "CSA[%u] " */
extern char D_00441558[];           /* "CLD[0x%x]\n" */

int printf(const char *, ...);
void *memcpy(void *, const void *, u32);

#define TEX0_BITS(header) ((TextureTex0Bits *)&(header)->tex0)

/* Builds, at *packet, the DMA packet that sends the texture at *texture to GS memory: a DMA tag and a
   VIF DIRECT code, the four transfer registers and the image as GIF IMAGE data, padded to quadwords,
   then the same for its CLUT when the pixel format has one. Prints the texture's TEX0 fields and
   leaves *packet after the data. Does nothing for a pixel format it does not know. */
void func_0020D478(char **texture, char **packet) {
    TextureHeader *header = (TextureHeader *)*texture;
    char *cursor = *packet;
    u64 *tags;
    char *bytes;
    u16 width;
    u16 height;
    int bits;
    int size;
    int quadwords;
    int clut_quadwords;
    int clut_width;
    int clut_height;
    int total;
    int clut_total;
    int base_pointer;
    int buffer_width;
    int image_format;
    int clut_offset;
    int has_clut;
    int i;
    u32 *tex0_words;
    u32 tex0_low;
    u32 tex0_high;

    width = 1 << TEX0_BITS(header)->width_log2;
    height = 1 << TEX0_BITS(header)->height_log2;
    switch (TEX0_BITS(header)->pixel_format) {
    case 0x00:
    case 0x30:
        bits = 32;
        break;
    case 0x01:
    case 0x31:
        bits = 24;
        break;
    case 0x02:
    case 0x0A:
    case 0x32:
    case 0x3A:
        bits = 16;
        break;
    case 0x13:
    case 0x1B:
        bits = 8;
        break;
    case 0x14:
    case 0x24:
    case 0x2C:
        bits = 4;
        break;
    default:
        return;
    }
    size = width * height * bits / 8;
    quadwords = (size + 15) / 16;
    if (bits == 4) {
        clut_quadwords = 2;
        if (TEX0_BITS(header)->clut_format == 0) {
            clut_quadwords = 4;
        }
        clut_width = 8;
        clut_height = 2;
    } else {
        clut_quadwords = 0x20;
        if (TEX0_BITS(header)->clut_format == 0) {
            clut_quadwords = 0x40;
        }
        clut_width = 16;
        clut_height = 16;
    }
    has_clut = bits < 9;
    if (has_clut) {
        clut_total = clut_quadwords + 12;       // match: its own variable, or the constant moves to the other operand
        total = quadwords + clut_total;
    } else {
        total = quadwords + 6;
    }
    // match: one cursor stepped through casts gives the separate increments; the CLUT part below is indexed
    *((u32 *)cursor)++ = (total + 1) | 0x70000000;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = 0;
    *((u32 *)cursor)++ = total | 0x51000000;
    *((u64 *)cursor)++ = 0x1000000000000004;
    *((u64 *)cursor)++ = 0xE;
    base_pointer = TEX0_BITS(header)->base_pointer;
    buffer_width = TEX0_BITS(header)->buffer_width;
    image_format = TEX0_BITS(header)->pixel_format;
    *((u64 *)cursor)++ = ((u64)base_pointer << 32) | ((u64)buffer_width << 48) | ((u64)image_format << 56);
    *((u64 *)cursor)++ = 0x50;
    *((u64 *)cursor)++ = 0;
    *((u64 *)cursor)++ = 0x51;
    *((u64 *)cursor)++ = width | ((u64)height << 32);
    *((u64 *)cursor)++ = 0x52;
    *((u64 *)cursor)++ = 0;
    *((u64 *)cursor)++ = 0x53;
    if (has_clut) {
        *((u64 *)cursor)++ = quadwords | (0x8000L << 44);
    } else {
        *((u64 *)cursor)++ = (quadwords | 0x8000) | (0x8000L << 44);
    }
    *((u64 *)cursor)++ = 0;
    bytes = cursor;
    memcpy(bytes, (char *)header + sizeof(TextureHeader), size);
    bytes += size;
    for (i = 0; i < quadwords * 16 - size; i++) {
        *bytes++ = 0;
    }
    if (has_clut) {
        tags = (u64 *)bytes;
        tags[0] = 0x1000000000000004;
        tags[1] = 0xE;
        tags[2] = ((u64)(int)TEX0_BITS(header)->clut_pointer << 32) | (0x8000L << 33) | ((u64)(int)TEX0_BITS(header)->clut_format << 56);
        tags[3] = 0x50;
        tags[4] = 0;
        tags[5] = 0x51;
        tags[6] = clut_width | ((u64)clut_height << 32);
        tags[7] = 0x52;
        tags[8] = 0;
        tags[9] = 0x53;
        tags[10] = (clut_quadwords | 0x8000) | (0x8000L << 44);
        tags[11] = 0;
        bytes += 12 * sizeof(u64);
        clut_offset = header->image_size + sizeof(TextureHeader);   // match: its own variable, as above
        memcpy(bytes, *texture + clut_offset, clut_quadwords * 16);
        bytes += clut_quadwords * 16;
    }
    // match: the dump reads TEX0 as two words kept in locals, through a pointer of its own (4(pointer) for the high word)
    tex0_words = (u32 *)&header->tex0;
    tex0_high = tex0_words[1];
    tex0_low = tex0_words[0];
    printf(D_004414B8);
    printf(D_004414C8, tex0_low & 0x3FFF);
    printf(D_004414D8, (tex0_low >> 14) & 0x3F);
    printf(D_004414E8, (tex0_low >> 20) & 0x3F);
    printf(D_00446FA8, (tex0_low >> 26) & 0xF);
    printf(D_00446FB0, (tex0_low >> 30) | ((tex0_high & 3) << 2));
    printf(D_004414F8, (tex0_high >> 2) & 1);
    printf(D_00441508, (tex0_high >> 3) & 3);
    printf(D_00441518, (tex0_high >> 5) & 0x3FFF);
    printf(D_00441528, (tex0_high >> 19) & 0xF);
    printf(D_00441538, (tex0_high >> 23) & 1);
    printf(D_00441548, (tex0_high >> 24) & 0x1F);
    printf(D_00441558, tex0_high >> 29);
    *packet = bytes;
}
