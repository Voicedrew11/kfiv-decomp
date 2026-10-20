/* GS registers, as passed to Packet_AddGsRegister (set a GS register). */
#ifndef KF_GS_H
#define KF_GS_H

#include "common.h"

#define GS_REG_PRIM         0x00
#define GS_REG_RGBAQ        0x01
#define GS_REG_UV           0x03
#define GS_REG_XYZ2         0x05
#define GS_REG_TEX0_1       0x06
#define GS_REG_CLAMP_1      0x08
#define GS_REG_TEX1_1       0x14
#define GS_REG_XYOFFSET_1   0x18
#define GS_REG_XYOFFSET_2   0x19
#define GS_REG_PRMODECONT   0x1A
#define GS_REG_PRMODE       0x1B
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
#define GS_REG_BITBLTBUF    0x50
#define GS_REG_TRXPOS       0x51
#define GS_REG_TRXREG       0x52
#define GS_REG_TRXDIR       0x53
#define GS_REG_FINISH       0x61

#define GIF_TAG_EOP         0x8000  /* GIF tag bit 15: the last tag of the packet */
#define GIF_TAG_PRIM_MASK   0x03FF8000 /* the PRIM field of a GIF tag: bits 47-57, bits 15-25 of its second word */
#define GS_PRIM_TME         0x10    /* PRIM bit 4: texture mapping */
#define GS_PRIM_CTXT_2      0x200   /* PRIM bit 9 (CTXT): the primitive draws with drawing context 2 */
#define GS_PRIM_SPRITE      6       /* the PRIM type of a sprite (bits 0-2) */
#define GS_PRIM_FLAT_SPRITE_MASK 0xFFFFFFE9 /* clears TME and the PRIM type bits of a sprite, which GS_PRIM_SPRITE sets again */
#define GS_ALPHA_C_FIX      0x20    /* ALPHA field C set to FIX: the alpha is the FIX field, bits 32-39 */
#define GS_ALPHA_CS_FIX_CD  0x68    /* ALPHA (Cs * FIX) + Cd: the blend that sets a frame's alpha */
#define GS_FRAME_FBW_640    0xA0000 /* FRAME FBW field (bits 16-21) of the 640-pixel-wide frame */
#define GS_DISPFB_FBW_MASK  0x7E00  /* DISPFB FBW field, bits 9-14 */
#define GS_FBW_MASK        0x3F        /* a buffer width value (FRAME, DISPFB), 6 bits */
#define GS_DISPFB_FBP_MASK  0x1FF   /* DISPFB FBP field, bits 0-8 */
#define GS_DISPFB_FBW_SHIFT 9       /* DISPFB FBW field, bits 9-14 */
#define GS_TEX0_BASE_MASK   0x3FFF      /* a 14-bit TEX0 base pointer value (TBP0 or CBP) */
#define GS_TEX0_TBP0(tex0)  ((tex0) & 0x3FFF)           /* TEX0 TBP0: texture base pointer, bits 0-13 */
#define GS_TEX0_TBW(tex0)   (((tex0) >> 14) & 0x3F)     /* TEX0 TBW: texture buffer width, bits 14-19 */
#define GS_TEX0_PSM(tex0)   (((tex0) >> 20) & 0x3F)     /* TEX0 PSM: texture pixel format, bits 20-25 */
#define GS_TEX0_TW(tex0)    (((tex0) >> 26) & 0xF)      /* TEX0 TW: texture width as log2, bits 26-29 */
#define GS_TEX0_CBP(tex0)   (((tex0) >> 37) & 0x3FFF)   /* TEX0 CBP: CLUT base pointer, bits 37-50 */
#define GS_TEX0_TH_WORDS(low, high) (((low) >> 30) | (((high) & 3) << 2)) /* TEX0 TH, bits 30-33, from the two words */
#define GS_TEX0_HI_TCC(high)  (((high) >> 2) & 1)      /* TEX0 TCC, bit 34 (bit 2 of the high word) */
#define GS_TEX0_HI_TFX(high)  (((high) >> 3) & 3)      /* TEX0 TFX, bits 35-36 */
#define GS_TEX0_HI_CBP(high)  (((high) >> 5) & 0x3FFF) /* TEX0 CBP, bits 37-50 */
#define GS_TEX0_HI_CPSM(high) (((high) >> 19) & 0xF)   /* TEX0 CPSM: CLUT pixel format, bits 51-54 */
#define GS_TEX0_HI_CSM(high)  (((high) >> 23) & 1)     /* TEX0 CSM: CLUT storage mode, bit 55 */
#define GS_TEX0_HI_CSA(high)  (((high) >> 24) & 0x1F)  /* TEX0 CSA: CLUT entry offset, bits 56-60 */
#define GS_TEX0_HI_CLD(high)  ((high) >> 29)           /* TEX0 CLD: CLUT load control, bits 61-63 */
#define SCREEN_XY_OUTSIDE_MASK 0xFFFF0000 /* x or y bits above the GS's 16-bit window: the point is outside it */
#define GIF_TAG_FLG_IMAGE   0x800000000000000 /* GIF tag FLG (bits 58-59) at 2, image mode: the qwords after the tag are image data */
#define GIF_TAG_PRIM_TME    0x80000 /* PRIM's TME bit in a GIF tag's second word (PRIM sits at bit 15): texture mapping */
#define GIF_TAG_PRIM_ABE    0x200000 /* PRIM's ABE bit in a GIF tag's second word: alpha blending */
#define GS_PSM_4BIT_PALETTE 0x4     /* PSM bit 2: the 4-bit palette formats (PSMT4 0x14, PSMT4HL 0x24, PSMT4HH 0x2C) */
#define GS_TEX0_TFX_DECAL   0x800000000 /* TEX0 TFX (bits 35-36) at 1: the texture is decal-blended over what is under it */

/* a color as four bytes, passed in an 8-byte argument (MenuText_SetColor stores it to text_ink, ui/menu_draw.c) */
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

/* a 64-bit value split into 16, 16, 24 and 8 bits, passed to MenuText_SetPen and kept
   in text_pen (ui/menu_draw.c); `raw` is the same word whole */
typedef union {
    s64 raw;
    struct {
        u64 lo16 : 16;              /* 0 */
        u64 mid16 : 16;             /* 16 */
        u64 mid24 : 24;             /* 32 */
        u64 hi8 : 8;                /* 56 */
    } f;
} GsPackedWord;

/* menu_frame_buffers: the menus' two frame buffers, a libgraph sceGsDBuffDc of which the game only names
   the colour each frame is cleared to */
typedef struct {
    char pad000[0x180];
    u64 clear0_rgbaq;               /* 0x180 sceGsClear.rgbaq of the first frame */
    char pad188[0x168];
    u64 clear1_rgbaq;               /* 0x2F0 of the second frame */
    char pad2F8[0x38];
} GsDoubleBufferDc;

/* GS registers as the bit-fields the game writes when it patches a double buffer (Display_Init) */
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

/* title_dbuff (ui/title.c), game_dbuff and game_dbuff_context2 (the game): a libgraph sceGsDBuff, two frame
   buffers with one context each. Other files name parts of the game's two: game_dbuff_draw0, game_dbuff_draw1,
   and game_dbuff_clear_color / game_dbuff_context2_clear_color, the RGBAQ of clear0 ([30] is that of clear1) */
typedef struct {
    GsDispEnvBits disp[2];          /* 0x000 */
    u64 giftag0[2];                 /* 0x050 */
    GsDrawEnvBits draw0;            /* 0x060 */
    GsClearBits clear0;             /* 0x0E0 */
    u64 giftag1[2];                 /* 0x140 */
    GsDrawEnvBits draw1;            /* 0x150 */
    GsClearBits clear1;             /* 0x1D0 */
} GsDoubleBuffer;

/* frame_dispfb_regs and frame_display_regs: the DISPFB and DISPLAY values of the two frames, kept by Display_Init */
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

/* a GIF packet drawing one sprite, sent by GsDraw_QueueTexturedSprite */
typedef struct {
    u64 tex0;                       /* 0x00 the TEX0 register */
    u64 gif_register_list;          /* 0x08 GS register list word of the sprite packet */
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

int MenuText_SetPen(GsPackedWord);

/* the rectangle MenuDraw_Bar draws: two corners, a color and a scale */
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

/* a screen corner as plain bit-fields of one 64-bit word (GsPackedWord without the union). The first field of
   such a struct is read with the alias set of its declared type, the others with alias set 0, and retail's
   load and store order in MenuDraw_Sprite and MenuDraw_Bar depends on exactly that */
typedef struct {
    u64 pos_x : 16;                 /* 0x00 12.4 fixed point */
    u64 pos_y : 16;                 /* 0x02 */
    u64 depth : 24;                 /* 0x04 */
    u64 top_byte : 8;  /* top byte of the packed GS word */
} GsCornerBits;

/* the textured rectangle MenuDraw_Sprite draws (a KindObject of kind 0): two corners with their texture
   coordinates, a colour, and the TEX0 register of the texture */
typedef struct {
    GsCornerBits first;             /* 0x00 top left */
    f32 first_s;                    /* 0x08 */
    f32 first_t;                    /* 0x0C */
    GsCornerBits second;            /* 0x10 bottom right */
    f32 second_s;                   /* 0x18 */
    f32 second_t;                   /* 0x1C */
    u32 red : 8;                    /* 0x20 */
    u32 green : 8;                  /* 0x21 */
    u32 blue : 8;                   /* 0x22 */
    u32 alpha : 8;                  /* 0x23 */
    f32 scale;                      /* 0x24 the Q of the RGBAQ register */
    s64 tex0;                       /* 0x28 */
} TexturedRectDef;

/* the flat rectangle MenuDraw_Bar draws, as that function reads it: RectDef (and the MenuCursorBar its
   callers fill) with the corners and the colour as bit-fields */
typedef struct {
    GsCornerBits first;             /* 0x00 top left */
    char pad08[0x08];
    GsCornerBits second;            /* 0x10 bottom right */
    char pad18[0x08];
    u32 red : 8;                    /* 0x20 */
    u32 green : 8;                  /* 0x21 */
    u32 blue : 8;                   /* 0x22 */
    u32 alpha : 8;                  /* 0x23 */
    f32 scale;                      /* 0x24 the Q of the RGBAQ register */
} FlatRectDef;

#endif
