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

/* a color as four bytes, passed in an 8-byte argument (func_001FDCD0 stores it to D_00446D70..D_00446D73) */
typedef struct {
    u8 red;                         /* 0x00 */
    u8 green;                       /* 0x01 */
    u8 blue;                        /* 0x02 */
    u8 alpha;                       /* 0x03 */
    char pad04[0x04];
} ColorBytes;

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

/* D_00426140: the menus' two frame buffers, a libgraph sceGsDBuffDc of which the game only names
   the colour each frame is cleared to */
typedef struct {
    char pad000[0x180];
    u64 clear0_rgbaq;               /* 0x180 sceGsClear.rgbaq of the first frame */
    char pad188[0x168];
    u64 clear1_rgbaq;               /* 0x2F0 of the second frame */
    char pad2F8[0x38];
} GsDoubleBufferDc;

/* GS registers as the bit-fields the game writes when it patches a double buffer (func_0020AFA8) */
typedef struct {
    u64 fbp : 9;
    u64 fbw : 6;
    u64 psm : 5;
    u64 pad20 : 12;
    u64 dbx : 11;
    u64 dby : 11;
    u64 pad54 : 10;
} GsDispfbBits;

typedef struct {
    u64 fbp : 9;
    u64 pad09 : 7;
    u64 fbw : 6;
    u64 pad22 : 2;
    u64 psm : 6;
    u64 pad30 : 2;
    u64 fbmsk : 32;
} GsFrameBits;

typedef struct {
    u64 zbp : 9;
    u64 pad09 : 15;
    u64 psm : 4;
    u64 pad28 : 4;
    u64 zmsk : 1;
    u64 pad33 : 31;
} GsZbufBits;

typedef struct {
    u64 ofx : 16;
    u64 pad16 : 16;
    u64 ofy : 16;
    u64 pad48 : 16;
} GsXyoffsetBits;

/* libgraph's sceGsDispEnv, sceGsDrawEnv1 and sceGsClear with those registers */
typedef struct {
    u64 pmode;                      /* 0x00 */
    u64 smode2;                     /* 0x08 */
    GsDispfbBits dispfb;            /* 0x10 */
    u64 display;                    /* 0x18 */
    u64 bgcolor;                    /* 0x20 */
} GsDispEnvBits;

typedef struct {
    GsFrameBits frame;              /* 0x00 */
    u64 frame_addr;                 /* 0x08 */
    GsZbufBits zbuf;                /* 0x10 */
    u64 zbuf_addr;                  /* 0x18 */
    GsXyoffsetBits xyoffset;        /* 0x20 */
    u64 xyoffset_addr;              /* 0x28 */
    u64 scissor;                    /* 0x30 */
    u64 scissor_addr;               /* 0x38 */
    u64 prmodecont;                 /* 0x40 */
    u64 prmodecont_addr;            /* 0x48 */
    u64 colclamp;                   /* 0x50 */
    u64 colclamp_addr;              /* 0x58 */
    u64 dthe;                       /* 0x60 */
    u64 dthe_addr;                  /* 0x68 */
    u64 test;                       /* 0x70 */
    u64 test_addr;                  /* 0x78 */
} GsDrawEnvBits;

typedef struct {
    u64 test_a;                     /* 0x00 */
    u64 test_a_addr;                /* 0x08 */
    u32 prim;                       /* 0x10 the low word; 0x200 is CTXT */
    u32 prim_hi;                    /* 0x14 */
    u64 prim_addr;                  /* 0x18 */
    u64 rgbaq;                      /* 0x20 */
    u64 rgbaq_addr;                 /* 0x28 */
    u64 xyz2_a;                     /* 0x30 */
    u64 xyz2_a_addr;                /* 0x38 */
    u64 xyz2_b;                     /* 0x40 */
    u64 xyz2_b_addr;                /* 0x48 */
    u64 test_b;                     /* 0x50 */
    u64 test_b_addr;                /* 0x58 */
} GsClearBits;

/* D_0042C4C0 (boot screens), D_00432D20 and D_00432F50 (the game): a libgraph sceGsDBuff, two frame
   buffers with one context each */
typedef struct {
    GsDispEnvBits disp[2];          /* 0x000 */
    u64 giftag0[2];                 /* 0x050 */
    GsDrawEnvBits draw0;            /* 0x060 */
    GsClearBits clear0;             /* 0x0E0 */
    u64 giftag1[2];                 /* 0x140 */
    GsDrawEnvBits draw1;            /* 0x150 */
    GsClearBits clear1;             /* 0x1D0 */
} GsDoubleBuffer;

/* D_00433180 and D_00433190: the DISPFB and DISPLAY values of the two frames, kept by func_0020AFA8 */
typedef struct {
    u64 first;
    u64 second;
} GsRegisterPair;

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

/* the rectangle func_001FC5E8 draws: two corners, a color and a scale */
typedef struct {
    GsPackedWord first;             /* 0x00 */
    char pad08[0x08];
    GsPackedWord second;            /* 0x10 */
    char pad18[0x08];
    u8 red;                         /* 0x20 */
    u8 green;                       /* 0x21 */
    u8 blue;                        /* 0x22 */
    u8 alpha;                       /* 0x23 */
    f32 scale;                      /* 0x24 */
} RectDef;

#endif
