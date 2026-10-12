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
    char pad24[0x1B];
    s8 slot;                        /* 0x3F which slot the packet belongs to */
    char pad40[0x01];
    s8 unk41;                       /* 0x41 */
    char pad42[0x1E];
} SpritePacket;

/* one corner of a ScreenSprite */
typedef struct {
    u16 x;                          /* 0x00 12.4 fixed point */
    u16 y;                          /* 0x02 */
    u32 z;                          /* 0x04 */
    s32 s;                          /* 0x08 texture coordinate, 12.4 fixed point (unused when untextured) */
    s32 t;                          /* 0x0C */
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
    u16 x0;                         /* 0x20 12.4 fixed point */
    u16 y0;                         /* 0x22 */
    u32 z0;                         /* 0x24 */
    s32 u0;                         /* 0x28 */
    s32 v0;                         /* 0x2C */
    u16 x1;                         /* 0x30 */
    u16 y1;                         /* 0x32 */
    u32 z1;                         /* 0x34 */
    s32 u1;                         /* 0x38 */
    s32 v1;                         /* 0x3C */
} ScreenQuad;

/* one vertex of the textured primitives func_00111368 builds: texture coordinates, then where it is on the screen */
typedef struct {
    f32 s;                          /* 0x00 */
    f32 t;                          /* 0x04 */
    f32 q;                          /* 0x08 1.0 */
    char pad0C[0x04];
    s32 x;                          /* 0x10 */
    s32 y;                          /* 0x14 */
    s32 z;                          /* 0x18 */
    s32 flags;                      /* 0x1C 0x8000: this vertex draws nothing */
} PacketStVertex;

#endif
