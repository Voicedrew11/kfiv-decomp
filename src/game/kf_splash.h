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

/* the 0x40-byte GS sprite packet for the splash image */
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

/* the effect shown when item `kind` is used (files item_ms/...), D_0042BBE0[kind] (kinds 0x46 to 0x52 are used) */
typedef struct {
    int unk00;                      /* 0x00 the motion frame at which func_00209480 goes to the next step */
    int unk04;                      /* 0x04 taken off the effect's brightness each frame while it fades (func_002094C8) */
    int unk08;                      /* 0x08 the motion frame at which the effect unk0C starts */
    int unk0C;                      /* 0x0C an effect number for func_001A1C58 */
} ItemUseEffectDef;

void KF_Splash_Reset(void);
int KF_Splash_GetState(void);
int KF_Splash_Start(int splash_id);
int KF_Splash_Begin(int splash_id, int hold_frames, int skippable, int fade_in_frames, int fade_out_frames);
void KF_Splash_Abort(void);
void KF_Splash_Update(void);
void KF_Splash_Draw(void);
void KF_Splash_BuildSprite(void);

#endif
