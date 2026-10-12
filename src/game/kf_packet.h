/* Prebuilt GS packets that the game copies and adjusts before sending. */
#ifndef KF_PACKET_H
#define KF_PACKET_H

#include "common.h"

/* the GS XYZ2 register: 12.4 fixed-point x and y, 32-bit z */
typedef struct {
    u64 X : 16;
    u64 Y : 16;
    u64 Z : 32;
} GsXyz;

/* a 0x60-byte sprite packet (two corners), D_005DD4C0/D_005DDB80/D_005DDC40 */
typedef struct {
    GsXyz xyz0;                     /* 0x00 */
    s64 unk08;                      /* 0x08 */
    GsXyz xyz1;                     /* 0x10 */
    char pad18[0x08];
    u8 red;                         /* 0x20 */
    u8 green;                       /* 0x21 */
    u8 blue;                        /* 0x22 */
    u8 alpha;                       /* 0x23 */
    char pad24[0x10];
    s32 unk34;                      /* 0x34 which number the sprite stands for (func_001FC820 gives its current value) */
    char pad38[0x06];
    u8 unk3E;                       /* 0x3E 1: a menu screen's own sprite, 2: one func_001D76D0/func_001D7760 draw */
    s8 slot;                        /* 0x3F which slot the packet belongs to */
    char pad40[0x01];
    s8 unk41;                       /* 0x41 */
    u8 unk42;                       /* 0x42 handed to func_001E5210 before the sprite is drawn */
    s8 unk43;                       /* 0x43 1: the cursor bar is drawn just before this one (func_001EEF78) */
    char pad44[0x1C];
} SpritePacket;

/* SpritePacket's colour (0x20) as four 8-bit fields of one unsigned word, the shape of Sony's sceGsRgbaq: a field
   read on its own is an unsigned int, so turning it into a float takes the unsigned conversion (func_001EA568) */
typedef struct {
    u32 red : 8;
    u32 green : 8;
    u32 blue : 8;
    u32 alpha : 8;
} SpritePacketColor;

#define SPRITE_PACKET_COLOR(packet) ((SpritePacketColor *)&(packet)->red)

/* the same four colour fields seen from the start of the packet: reads and writes are then based on the packet's
   own address (lbu 0x23(packet)), as retail's are in func_001E02F8 */
typedef struct {
    char pad00[0x20];
    u32 red : 8;                    /* 0x20 */
    u32 green : 8;                  /* 0x21 */
    u32 blue : 8;                   /* 0x22 */
    u32 alpha : 8;                  /* 0x23 */
    char pad24[0x3C];
} SpritePacketColorView;

#define SPRITE_PACKET_COLOR_VIEW(packet) ((SpritePacketColorView *)(packet))

/* one quadword DMA tag at the head of a packet chain */
typedef struct {
    s32 id_and_qwc;                 /* 0x00 the DMA tag ID (bits 28-30) and quadword count */
    s32 address;                    /* 0x04 the next packet, or 0 */
    s32 unk08;                      /* 0x08 */
    s32 unk0C;                      /* 0x0C */
} DmaTag;

/* one corner of a ScreenSprite */
typedef struct {
    u16 x;                          /* 0x00 12.4 fixed point */
    u16 y;                          /* 0x02 */
    u32 z;                          /* 0x04 */
    s32 tex_s;                          /* 0x08 texture coordinate, 12.4 fixed point (unused when untextured) */
    s32 tex_t;                          /* 0x0C */
} ScreenSpriteVertex;

/* what func_0010A100 (textured) and func_0010A338 (flat) turn into a GS sprite; same layout as SplashSprite */
typedef struct {
    u64 tex0;                       /* 0x00 GS TEX0 */
    u64 tex1;                       /* 0x08 GS TEX1 */
    s32 prim;                       /* 0x10 GS PRIM bits; the sprite type (6) is added */
    u8 rgba[4];                     /* 0x14 */
    char pad18[0x08];
    ScreenSpriteVertex vertex[2];   /* 0x20 */
} ScreenSprite;

/* a screen rectangle with two corners, as func_0010A100 draws it */
typedef struct {
    s64 tag;                        /* 0x00 */
    s64 unk08;                      /* 0x08 */
    s32 unk10;                      /* 0x10 */
    u8 red;                         /* 0x14 */
    u8 green;                       /* 0x15 */
    u8 blue;                        /* 0x16 */
    u8 alpha;                       /* 0x17 */
    char pad18[0x08];
    u16 first_x;                         /* 0x20 12.4 fixed point */
    u16 first_y;                         /* 0x22 */
    u32 first_z;                         /* 0x24 */
    s32 first_u;                         /* 0x28 */
    s32 first_v;                         /* 0x2C */
    u16 second_x;                         /* 0x30 */
    u16 second_y;                         /* 0x32 */
    u32 second_z;                         /* 0x34 */
    s32 second_u;                         /* 0x38 */
    s32 second_v;                         /* 0x3C */
} ScreenQuad;

/* one vertex of the textured primitives func_00111368 builds: texture coordinates, then where it is on the screen */
typedef struct {
    f32 tex_s;                          /* 0x00 */
    f32 tex_t;                          /* 0x04 */
    f32 tex_q;                          /* 0x08 1.0 */
    char pad0C[0x04];
    s32 x;                          /* 0x10 */
    s32 y;                          /* 0x14 */
    s32 z;                          /* 0x18 */
    s32 flags;                      /* 0x1C 0x8000: this vertex draws nothing */
} PacketStVertex;

/* the GS registers a draw environment sets, each followed by its register address (SDK sceGsDrawEnv1) */
typedef struct {
    s64 frame;                      /* 0x00 */
    s64 frame_addr;                 /* 0x08 */
    s64 zbuf;                       /* 0x10 */
    s64 zbuf_addr;                  /* 0x18 */
    s64 xyoffset;                   /* 0x20 */
    s64 xyoffset_addr;              /* 0x28 */
    s64 scissor;                    /* 0x30 */
    s64 scissor_addr;               /* 0x38 */
    s64 prmodecont;                 /* 0x40 */
    s64 prmodecont_addr;            /* 0x48 */
    s64 colclamp;                   /* 0x50 */
    s64 colclamp_addr;              /* 0x58 */
    s64 dthe;                       /* 0x60 */
    s64 dthe_addr;                  /* 0x68 */
    s64 test;                       /* 0x70 */
    s64 test_addr;                  /* 0x78 */
} GsDrawEnv;

/* a GIF tag followed by a draw environment, the packet sceGsPutDrawEnv sends */
typedef struct {
    int tag[4];                     /* 0x00 */
    GsDrawEnv env;                  /* 0x10 */
} GsDrawPacket;

/* 32 bytes of GS data that D_0043EB90 holds */
typedef struct {
    s64 unk00;                      /* 0x00 */
    s64 unk08;                      /* 0x08 */
    s64 unk10;                      /* 0x10 */
    s64 unk18;                      /* 0x18 */
} GsDataBlock;

/* the local frame func_0019B360 and func_0019B4B0 build: a cleared header, a data block, the packet */
typedef struct {
    char pad00[0x10];
    GsDataBlock block;              /* 0x10 */
    GsDrawPacket packet;            /* 0x30 */
} GsDrawFrame;

/* one 32-bit word of a packet built in place (func_001E5070), written as an integer or as a float */
typedef union {
    s32 as_int;
    f32 as_float;
} GsPacketWord;

int sceGsPutDrawEnv(GsDrawPacket *);
int sceGsSetDefDrawEnv(GsDrawEnv *, s16, s16, s16, s16, s16);
int sceGsSyncPath(int, int);

#endif
