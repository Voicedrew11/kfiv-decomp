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

/* a texture func_0010A880 redraws into another one every frame, scrolled (D_00474AC0[4]) */
typedef struct {
    u16 width;                      /* 0x00 of the source texture, a power of two */
    u16 height;                     /* 0x02 */
    u16 psm;                        /* 0x04 TEX0 fields of the source: pixel format */
    u16 tbp;                        /* 0x06 base pointer */
    u16 cpsm;                       /* 0x08 CLUT pixel format */
    u16 cbp;                        /* 0x0A CLUT base pointer */
    f32 repeat_x;                   /* 0x0C how many times the source spans the target */
    f32 repeat_y;                   /* 0x10 */
    u16 target_width;               /* 0x14 */
    u16 target_height;              /* 0x16 */
    u16 target_psm;                 /* 0x18 */
    u16 target_fbp;                 /* 0x1A the target's base pointer */
    u8 rgba[4];                     /* 0x1C */
    u8 alpha_a : 2;                 /* 0x20 the ALPHA register's four sources */
    u32 alpha_b : 2;
    u32 alpha_c : 2;
    u32 alpha_d : 2;
    u8 alpha_fix;                   /* 0x21 */
    u8 period;                      /* 0x22 scrolls every this many frames, 0: every frame */
    char pad23;
    s16 step_x;                     /* 0x24 scroll per step, 12.4 fixed point */
    s16 step_y;                     /* 0x26 */
} ScrollTexture;

/* where a ScrollTexture has scrolled to (D_00474B60[4]) */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
} ScrollOffset;

typedef struct {
    int offset;                     /* 0x00 from the table base, to a chunk: its size in quadwords, 12 bytes, the data */
    char pad04[0x08];
    int unk0C;                      /* 0x0C not 0: func_0010B2E0 skips it (set by func_0010B4E0) */
} TextureTableEntry;

#endif
