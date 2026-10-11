/* Splash images (msg/mNNNN.tm2): loaded, faded in, held and faded out by a state machine. */
#ifndef KF_SPLASH_H
#define KF_SPLASH_H

#include "common.h"

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

int func_00208198(int splash_id, int hold_frames, int skippable, int fade_in_frames, int fade_out_frames);
void func_00208670(void);

#endif
