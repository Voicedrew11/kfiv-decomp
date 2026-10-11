/* Texture data: image headers carrying a GS TEX0 value, and chunk tables. */
#ifndef KF_TEXTURE_H
#define KF_TEXTURE_H

#include "common.h"

/* a 0x20-byte image header; the image (and then its CLUT) follows it */
typedef struct {
    int data_offset;                /* 0x00 from this header */
    int image_size;                 /* 0x04 bytes */
    char pad08[0x08];
    u64 tex0;                       /* 0x10 GS TEX0: TBP0 at bit 0, CBP at bit 37 */
    char pad18[0x08];
} TextureHeader;

/* a chunk table: this header, then entry_count TextureTableEntry */
typedef struct {
    int unk00;                      /* 0x00 */
    u32 entry_count;                /* 0x04 */
    char pad08[0x08];
} TextureTableHeader;

typedef struct {
    int offset;                     /* 0x00 from the table base */
    char pad04[0x0C];
} TextureTableEntry;

#endif
