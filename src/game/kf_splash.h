/* Splash images (msg/mNNNN.tm2): loaded, faded in, held and faded out by a state machine. */
#ifndef KF_SPLASH_H
#define KF_SPLASH_H

#include "common.h"
#include "kf_texture.h"

/* the image file as loaded, at D_00249680 */
typedef struct {
    u64 unk00;                      /* 0x00 */
    u64 unk08;                      /* 0x08 */
    u64 tex0;                       /* 0x10 GS TEX0 of the image */
    u64 unk18;                      /* 0x18 */
} SplashImage;

/* the 0x40-byte GS sprite packet for the splash image, D_005F9B00 */
typedef struct {
    u64 tex0;                       /* 0x00 GS TEX0 */
    u64 unk08;                      /* 0x08 */
    int gif_reg;                    /* 0x10 GIF register field (0x156) */
    u8 red;                         /* 0x14 */
    u8 green;                       /* 0x15 */
    u8 blue;                        /* 0x16 */
    u8 alpha;                       /* 0x17 brightness, 0..0x80 */
    char pad18[0x08];
    u16 vertex0_x;                  /* 0x20 */
    u16 vertex0_y;                  /* 0x22 */
    u32 vertex0_z;                  /* 0x24 */
    int vertex0_s;                  /* 0x28 */
    int vertex0_t;                  /* 0x2C */
    u16 vertex1_x;                  /* 0x30 */
    u16 vertex1_y;                  /* 0x32 */
    u32 vertex1_z;                  /* 0x34 */
    int vertex1_s;                  /* 0x38 texture width << 4 */
    int vertex1_t;                  /* 0x3C texture height << 4 */
} SplashSprite;

extern SplashImage D_00249680;
extern SplashSprite D_005F9B00;

/* how one sprite of the boot screens (language choice, title logos) changes over time: four keys */
typedef struct {
    int key_frames[4];              /* 0x00 the frame each key starts at */
    int key_values[4];              /* 0x10 the brightness at each key, 0..0x80 */
    int length;                     /* 0x20 the frame counter starts over after this many frames */
} BootSpriteTrack;

/* one sprite of the boot screens, set up by func_00209A28 and drawn by func_00209A48 */
typedef struct {
    int texture;                    /* 0x00 index into D_005FAB60 */
    int pos_x;                      /* 0x04 left edge, from the screen centre */
    int pos_y;                      /* 0x08 top edge in frame lines (halved when drawn), from the screen centre */
    int width;                      /* 0x0C */
    int height;                     /* 0x10 in frame lines */
    int frame;                      /* 0x14 counts up to track->length */
    BootSpriteTrack *track;         /* 0x18 */
} BootSprite;

/* the GS TEX0 register as bit-fields */
typedef struct {
    u64 tbp0 : 14;                  /* bit 0 where the texture is in GS memory */
    u64 tbw : 6;                    /* bit 14 */
    u64 psm : 6;                    /* bit 20 pixel format */
    u64 tw : 4;                     /* bit 26 log2 of the width */
    u64 th : 4;                     /* bit 30 log2 of the height */
    u64 tcc : 1;                    /* bit 34 */
    u64 tfx : 2;                    /* bit 35 */
    u64 cbp : 14;                   /* bit 37 where the palette is */
    u64 cpsm : 4;                   /* bit 51 palette pixel format */
    u64 csm : 1;                    /* bit 55 */
    u64 csa : 5;                    /* bit 56 */
    u64 cld : 3;                    /* bit 61 */
} BootTex0;

/* SplashImage (a TextureHeader) with its TEX0 as bit-fields */
typedef struct {
    int data_offset;                /* 0x00 */
    int image_size;                 /* 0x04 bytes of pixels; the palette follows them */
    char pad08[0x08];
    BootTex0 tex0;                  /* 0x10 */
    u64 unk18;                      /* 0x18 */
} SplashImageBits;

/* the 0x70 bytes func_002087D0 puts in front of image data it sends to GS memory: five registers, then the tag of the data */
typedef struct {
    u64 tag[2];                     /* 0x00 GIF tag: five address+data pairs */
    u32 texflush[2];                /* 0x10 */
    u32 texflush_reg[2];            /* 0x18 0x3F, TEXFLUSH */
    u64 bitbltbuf;                  /* 0x20 where the data goes: base pointer, buffer width, pixel format */
    u32 bitbltbuf_reg[2];           /* 0x28 0x50, BITBLTBUF */
    u64 trxpos;                     /* 0x30 */
    u32 trxpos_reg[2];              /* 0x38 0x51, TRXPOS */
    u64 trxreg;                     /* 0x40 width, and height in the high half */
    u32 trxreg_reg[2];              /* 0x48 0x52, TRXREG */
    u64 trxdir;                     /* 0x50 */
    u32 trxdir_reg[2];              /* 0x58 0x53, TRXDIR */
    u64 image_tag;                  /* 0x60 GIF tag of the data that follows, in quadwords */
    u32 image_tag_high[2];          /* 0x68 */
} SplashLoadPacket;

/* one texture of the boot screens, D_005FAB60[13], loaded by func_002097E0 */
typedef struct {
    BootTex0 tex0;                  /* 0x00 GS TEX0, copied from the file */
    TextureHeader *file;            /* 0x08 the file as loaded */
} BootTexture;

/* the GS packet func_00209A48 draws a boot screen sprite with, D_005F9B60 */
typedef struct {
    u32 tag[4];                     /* 0x00 GIF tag: two address+data pairs */
    u64 alpha;                      /* 0x10 from D_0042C1C0 by blend mode */
    u64 alpha_reg;                  /* 0x18 0x42, ALPHA_1 */
    u64 test;                       /* 0x20 */
    u64 test_reg;                   /* 0x28 0x47, TEST_1 */
    u32 list_tag[4];                /* 0x30 GIF tag: a list of eight registers */
    u32 prim[2];                    /* 0x40 a textured sprite with alpha blending */
    BootTex0 tex0;                  /* 0x48 */
    u32 rgba;                       /* 0x50 */
    f32 rgba_q;                     /* 0x54 */
    f32 first_s;                    /* 0x58 texture coordinates of the top left corner */
    f32 first_t;                    /* 0x5C */
    u32 first_xy;                   /* 0x60 x and y in 16ths of a pixel, y in the high half */
    u32 first_z;                    /* 0x64 */
    f32 second_s;                   /* 0x68 the bottom right corner */
    f32 second_t;                   /* 0x6C */
    u32 second_xy;                  /* 0x70 */
    u32 second_z;                   /* 0x74 */
    u32 nop[2];                     /* 0x78 */
} BootSpritePacket;

/* the effect shown when item `kind` is used (files item_ms/...), D_0042BBE0[kind] (kinds 0x46 to 0x52 are used) */
typedef struct {
    int unk00;                      /* 0x00 the motion frame at which func_00209480 goes to the next step */
    int unk04;                      /* 0x04 taken off the effect's brightness each frame while it fades (func_002094C8) */
    int unk08;                      /* 0x08 the motion frame at which the effect unk0C starts */
    int unk0C;                      /* 0x0C an effect number for func_001A1C58 */
} ItemUseEffectDef;

int func_00208198(int splash_id, int hold_frames, int skippable, int fade_in_frames, int fade_out_frames);
void func_00208670(void);

#endif
