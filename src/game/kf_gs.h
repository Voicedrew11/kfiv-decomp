/* GS registers, as passed to func_00109A80 (set a GS register). */
#ifndef KF_GS_H
#define KF_GS_H

#include "common.h"

#define GS_REG_TEX0_1       0x06
#define GS_REG_CLAMP_1      0x08
#define GS_REG_TEX1_1       0x14
#define GS_REG_XYOFFSET_1   0x18
#define GS_REG_XYOFFSET_2   0x19
#define GS_REG_SCANMSK      0x22
#define GS_REG_TEXFLUSH     0x3F
#define GS_REG_SCISSOR_1    0x40
#define GS_REG_SCISSOR_2    0x41
#define GS_REG_ALPHA_1      0x42
#define GS_REG_ALPHA_2      0x43
#define GS_REG_TEST_1       0x47
#define GS_REG_TEST_2       0x48
#define GS_REG_FRAME_1      0x4C
#define GS_REG_FRAME_2      0x4D
#define GS_REG_ZBUF_1       0x4E
#define GS_REG_ZBUF_2       0x4F
#define GS_REG_FINISH       0x61

/* the ALPHA registers (GS_REG_ALPHA_1/2) as a value to write */
typedef union {
    s64 raw;
    struct {
        u64 a_source : 2;           /* 0 */
        u64 b_source : 2;           /* 2 */
        u64 c_source : 2;           /* 4 */
        u64 d_source : 2;           /* 6 */
        u64 pad8 : 24;
        u64 fix : 8;                /* 32 */
        u64 pad40 : 24;
    } f;
} GsAlphaReg;

/* a 64-bit value split into 16, 16, 24 and 8 bits, passed to func_001FDB88 and kept
   in D_00446D60; `raw` is the same word whole */
typedef union {
    s64 raw;
    struct {
        u64 lo16 : 16;              /* 0 */
        u64 mid16 : 16;             /* 16 */
        u64 mid24 : 24;             /* 32 */
        u64 hi8 : 8;                /* 56 */
    } f;
} GsPackedWord;

/* one corner of the sprite in a GsSpritePacket */
typedef struct {
    u16 x;                          /* 0x00 screen position, 12.4 fixed point */
    u16 y;                          /* 0x02 */
    u32 z;                          /* 0x04 */
    int texel_x;                    /* 0x08 texel position, 12.4 fixed point */
    int texel_y;                    /* 0x0C */
} GsSpriteCorner;

/* a GIF packet drawing one sprite, sent by func_0010A100 */
typedef struct {
    u64 tex0;                       /* 0x00 the TEX0 register */
    u64 unk08;                      /* 0x08 */
    int prim;                       /* 0x10 */
    u8 rgba[4];                     /* 0x14 */
    u64 pad18;
    GsSpriteCorner corner[2];       /* 0x20 */
} GsSpritePacket;

/* a colour as four 32-bit RGBA words, copied into a packet as one quadword */
typedef struct {
    int rgba[4];
} GsColor;

/* a screen rectangle in pixels */
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} ScreenRect;

int func_001FDB88(GsPackedWord);

#endif
