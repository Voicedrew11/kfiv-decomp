/* Texture uploads: sending a texture (a TextureHeader and its image, as in a .tm2 chunk table) to GS memory
   directly (Texture_LoadTm2, "load_tm2()" in its message) or building DMA packets that do it, moving the base
   pointers of a table of them, and a TEXFLUSH. */
#include "gfx/gs.h"
#include "gfx/texture.h"
#include <eeregs.h>
#include <libgraph.h>
#include "core/file.h"
#include <eekernel.h>
#include <string.h>

#define TEX0_BITS(header) ((TextureTex0Bits *)&(header)->tex0)

extern char tm2_tbp_zero_message[]; /* "load_tm2() tbp:0 !!!!" */
extern char tm2_header_label[];     /* "load_tm2()" */
extern char tex0_tbp_format[];      /* "TBP[%u] " */
extern char tex0_tbw_format[];      /* "TBW[%u] " */
extern char tex0_psm_format[];      /* "PSM[0x%x] " */
extern char tex0_tw_format[];       /* "TW[%u] " */
extern char tex0_th_format[];       /* "TH[%u] " */
extern char tex0_tcc_format[];      /* "TCC[%u] " */
extern char tex0_tfx_format[];      /* "TFX[0x%x] " */
extern char tex0_cbp_format[];      /* "CBP[%u] " */
extern char tex0_cpsm_format[];     /* "CPSM[0x%x] " */
extern char tex0_csm_format[];      /* "CSM[%u] " */
extern char tex0_csa_format[];      /* "CSA[%u] " */
extern char tex0_cld_format[];      /* "CLD[0x%x]
" */
static u32 texflush_packet[0xC];    /* 0x1DFAF80 where Texture_SendTexFlush builds its DMA packet */

static void Texture_LoadTm2(TextureHeader *header);
static void Texture_BuildPacket(char **texture, char **packet);

/* Uploads a texture chunk when asked to; returns the address after it. */
static char *Texture_UploadChunk(char *chunk, int upload) {
    TextureHeader *header = (TextureHeader *)chunk;

    if (upload) {
        Texture_LoadTm2(header);
    }
    return chunk + header->data_offset;
}

/* load_tm2(): sends a texture's image to GS memory, and its CLUT when the pixel format has one; waits for each
   transfer. Prints a message and does nothing when the base pointer is 0. */
static void Texture_LoadTm2(TextureHeader *header) {
    sceGsLoadImage load_image;
    int pixel_format;
    u16 buffer_width;
    u16 image_format;
    s16 width;
    s16 height;

    if (TEX0_BITS(header)->base_pointer == 0) {
        puts(tm2_tbp_zero_message);
        return;
    }
    image_format = TEX0_BITS(header)->pixel_format;
    buffer_width = TEX0_BITS(header)->buffer_width;
    width = 1 << TEX0_BITS(header)->width_log2;
    height = 1 << TEX0_BITS(header)->height_log2;
    sceGsSetDefLoadImage(&load_image, (u16)TEX0_BITS(header)->base_pointer, buffer_width, image_format, 0, 0, width, height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + sizeof(TextureHeader));
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    pixel_format = TEX0_BITS(header)->pixel_format;
    if (pixel_format == 0x13 || pixel_format == 0x1B) {
        sceGsSetDefLoadImage(&load_image, (u16)TEX0_BITS(header)->clut_pointer, 1, (u16)TEX0_BITS(header)->clut_format, 0, 0, 16, 16);
        FlushCache(0);
        sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
        }
    } else if (pixel_format == 0x14 || pixel_format == 0x24 || pixel_format == 0x2C) {
        sceGsSetDefLoadImage(&load_image, (u16)TEX0_BITS(header)->clut_pointer, 1, (u16)TEX0_BITS(header)->clut_format, 0, 0, 8, 2);
        FlushCache(0);
        sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
        }
    }
}

/* Builds, at *packets, a table of GS upload packets from the texture table at *textures: the same
   header and entries, then one packet per texture (Texture_BuildPacket). Leaves *packets after the last
   packet, and the size of the whole in the new header's first word. Nothing calls it. */
static void Texture_BuildPacketTable(char **textures, char **packets) {
    char *source_cursor;
    char *target_cursor;
    TextureTableHeader *source;
    TextureTableHeader *target;
    TextureTableEntry *source_entry;
    TextureTableEntry *target_entry;
    u32 i;

    source_cursor = *textures;
    target_cursor = *packets;
    source = (TextureTableHeader *)source_cursor;
    source_cursor += sizeof(TextureTableHeader);
    source_entry = (TextureTableEntry *)source_cursor;
    source_cursor += source->entry_count * sizeof(TextureTableEntry);
    target = (TextureTableHeader *)target_cursor;
    target->packet_size = 0;
    target_cursor += sizeof(TextureTableHeader);
    target_entry = (TextureTableEntry *)target_cursor;
    target->entry_count = source->entry_count;
    target->spare_word_a = 0;
    target->spare_word_b = 0;
    target_cursor += target->entry_count * sizeof(TextureTableEntry);
    for (i = 0; i < source->entry_count; i++) {
        source_cursor = *textures + source_entry[i].offset;
        target_entry[i].offset = target_cursor - *packets;
        target_entry[i].disabled = 0;
        target_entry[i].spare_word_b = 0;
        target_entry[i].spare_word_a = 0;
        Texture_BuildPacket(&source_cursor, &target_cursor);
    }
    target->packet_size = target_cursor - *packets;
    *packets = target_cursor;
}

/* Calls Texture_BuildPacket with the same arguments. */
static void Texture_BuildPacketWrapper(char **texture, char **packet) {
    Texture_BuildPacket(texture, packet);
}

/* Builds, at *packet, the DMA packet that sends the texture at *texture to GS memory: a DMA tag and a
   VIF DIRECT code, the four transfer registers and the image as GIF IMAGE data, padded to quadwords,
   then the same for its CLUT when the pixel format has one. Prints the texture's TEX0 fields and
   leaves *packet after the data. Does nothing for a pixel format it does not know. */
static void Texture_BuildPacket(char **texture, char **packet) {
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
    *((u32 *)cursor)++ = (total + 1) | DMA_TAG_ID_END;
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
        *((u64 *)cursor)++ = (quadwords | GIF_TAG_EOP) | (0x8000L << 44);
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
        tags[10] = (clut_quadwords | GIF_TAG_EOP) | (0x8000L << 44);
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
    printf(tm2_header_label);
    printf(tex0_tbp_format, GS_TEX0_TBP0(tex0_low));
    printf(tex0_tbw_format, GS_TEX0_TBW(tex0_low));
    printf(tex0_psm_format, GS_TEX0_PSM(tex0_low));
    printf(tex0_tw_format, GS_TEX0_TW(tex0_low));
    printf(tex0_th_format, GS_TEX0_TH_WORDS(tex0_low, tex0_high));
    printf(tex0_tcc_format, GS_TEX0_HI_TCC(tex0_high));
    printf(tex0_tfx_format, GS_TEX0_HI_TFX(tex0_high));
    printf(tex0_cbp_format, GS_TEX0_HI_CBP(tex0_high));
    printf(tex0_cpsm_format, GS_TEX0_HI_CPSM(tex0_high));
    printf(tex0_csm_format, GS_TEX0_HI_CSM(tex0_high));
    printf(tex0_csa_format, GS_TEX0_HI_CSA(tex0_high));
    printf(tex0_cld_format, GS_TEX0_HI_CLD(tex0_high));
    *packet = bytes;
}

/* Sends the GS a TEXFLUSH through a DMA chain on channel 2 (GIF): an end tag, a GIF tag of one A+D register,
   and the register write, built at texflush_packet. Returns at once, without waiting for the transfer. */
void Texture_SendTexFlush(void) {
    u32 *cursor;

    cursor = (u32 *)texflush_packet;
    *cursor++ = DMA_TAG_ID_END | 2;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x8001;
    *cursor++ = 0x10004000;
    *cursor++ = 0xE;
    *cursor++ = 0;
    *cursor++ = 0xFFFFFFFFU;
    *cursor++ = 0;
    *cursor++ = GS_REG_TEXFLUSH;
    *cursor = 0;
    FlushCache(0);
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_MADR = 0;
    *D2_TADR = (int)texflush_packet;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
}

/* Returns 0. */
static int Texture_ReturnZero(void) {
    return 0;
}

/* Walks a chunk table, handing each entry's chunk to Texture_UploadChunk (uploaded when `upload` is set);
   returns the address after the last chunk. */
char *Texture_UploadTable(char *base, int upload) {
    char *cursor;
    TextureTableHeader *header;
    TextureTableEntry *entry;
    u32 i;

    cursor = base;
    header = (TextureTableHeader *)cursor;
    cursor += sizeof(TextureTableHeader);
    entry = (TextureTableEntry *)cursor;
    cursor += header->entry_count * sizeof(TextureTableEntry);
    for (i = 0; i < header->entry_count; i++) {
        cursor = Texture_UploadChunk((char *)(entry[i].offset + (int)base), upload);
    }
    return cursor;
}

/* Moves the texture's base pointer (TBP0) by tbp_delta and its CLUT's (CBP) by cbp_delta, each kept to 14 bits;
   returns the address after the texture. */
static void *Texture_MoveBasePointers(TextureHeader *header, int tbp_delta, int cbp_delta) {
    u64 bits = header->tex0;
    int tbp;
    int cbp;

    tbp = ((int)GS_TEX0_TBP0(bits) + tbp_delta) & GS_TEX0_BASE_MASK;
    bits &= ~(u64)GS_TEX0_BASE_MASK;
    bits |= tbp;
    cbp = (int)GS_TEX0_CBP(bits);
    bits &= 0xFFF8001FFFFFFFFFUL;
    bits |= (u64) ((cbp + cbp_delta) & GS_TEX0_BASE_MASK) << 37;
    header->tex0 = bits;
    return (char *) header + header->data_offset;
}

/* Texture_MoveBasePointers for every texture of a chunk table; returns the address after the last one.
   Nothing calls it. */
static char *Texture_MoveTableBasePointers(char *base, int tbp_delta, int cbp_delta) {
    char *cursor;
    TextureTableHeader *header;
    TextureTableEntry *entry;
    u32 i;

    cursor = base;
    header = (TextureTableHeader *)cursor;
    cursor += sizeof(TextureTableHeader);
    entry = (TextureTableEntry *)cursor;
    cursor += header->entry_count * sizeof(TextureTableEntry);
    for (i = 0; i < header->entry_count; i++) {
        cursor = Texture_MoveBasePointers((TextureHeader *)(entry[i].offset + (int)base), tbp_delta, cbp_delta);
    }
    return cursor;
}
