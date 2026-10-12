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

int sceGsPutDrawEnv(GsDrawPacket *);
int sceGsSetDefDrawEnv(GsDrawEnv *, s16, s16, s16, s16, s16);
int sceGsSyncPath(int, int);

#endif
