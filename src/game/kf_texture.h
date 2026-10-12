/* Texture data: image headers carrying a GS TEX0 value, and chunk tables. */
#ifndef KF_TEXTURE_H
#define KF_TEXTURE_H

#include "common.h"

/* a 0x20-byte image header; the image (and then its CLUT) follows it */
typedef struct {
    int data_offset;                /* 0x00 from this header */
    int image_size;                 /* 0x04 bytes */
    char pad08[0x04];
    int unk0C;                      /* 0x0C from this header to a second CLUT (func_002048D8) */
    u64 tex0;                       /* 0x10 GS TEX0: TBP0 at bit 0, CBP at bit 37 */
    u64 unk18;                      /* 0x18 copied to PoolSlot.unk60 with tex0 (func_001DC2A8) */
} TextureHeader;

/* the fields of a GS TEX0 value (TextureHeader.tex0), as func_0020D478 reads them */
typedef struct {
    u64 base_pointer : 14;          /* TBP0 */
    u64 buffer_width : 6;           /* TBW */
    u64 pixel_format : 6;           /* PSM */
    u64 width_log2 : 4;             /* TW */
    u64 height_log2 : 4;            /* TH */
    u64 has_alpha : 1;              /* TCC */
    u64 function : 2;               /* TFX */
    u64 clut_pointer : 14;          /* CBP */
    u64 clut_format : 4;            /* CPSM */
    u64 clut_mode : 1;              /* CSM */
    u64 clut_offset : 5;            /* CSA */
    u64 clut_load : 3;              /* CLD */
} TextureTex0Bits;

/* a TextureHeader with the two base pointers of its TEX0 as bit-fields; `raw` is the same register whole */
typedef struct {
    char pad00[0x10];
    union {
        u64 raw;
        struct {
            u64 tbp0 : 14;          /* bit 0 */
            u64 pad14 : 23;
            u64 cbp : 14;           /* bit 37 */
            u64 pad51 : 13;
        } f;
    } tex0;                         /* 0x10 */
    union {
        u64 raw;
        s64 as_signed;
    } unk18;                        /* 0x18 a union so that, like tex0, reading it counts as touching anything (func_001DC2A8) */
} TextureHeaderBits;

#define TEXTURE_HEADER_BITS(header) ((TextureHeaderBits *)(header))

/* D_0042A8D0: a loaded texture file (the menu font) and the two words func_00204D60 reads from it */
typedef struct {
    u64 tex0;                       /* 0x00 */
    u64 trailer;                    /* 0x08 the word after its CLUT */
    TextureHeader *header;          /* 0x10 */
} LoadedTexture;

/* a chunk table: this header, then entry_count TextureTableEntry */
typedef struct {
    int unk00;                      /* 0x00 */
    u32 entry_count;                /* 0x04 */
    int unk08;                      /* 0x08 func_0020D348 clears it */
    int unk0C;                      /* 0x0C func_0020D348 clears it */
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
    int unk04;                      /* 0x04 func_0020D348 clears it */
    int unk08;                      /* 0x08 func_0020D348 clears it */
    int unk0C;                      /* 0x0C not 0: func_0010B2E0 skips it (set by func_0010B4E0) */
} TextureTableEntry;

#endif
