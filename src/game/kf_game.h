/* Game-wide state blocks. */
#ifndef KF_GAME_H
#define KF_GAME_H

#include "common.h"

/* D_0038CC50 (cleared as 0x48 bytes by func_001127F8). Other files still
   declare it as char[]/int[], so the extern stays in each file. */
typedef struct {
    char unk00;                     /* 0x00 nonzero: script blends are skipped */
    char unk01;                     /* 0x01 */
    char pad02;
    s8 unk03;                       /* 0x03 */
    u8 unk04;                       /* 0x04 0 or 1 */
    char pad05[0x02];
    char unk07;                     /* 0x07 */
    char pad08[0x1C];
    u32 bit0 : 1;                   /* 0x24 */
    u32 bit1 : 1;                   /* set while func_0011FDD0 runs an event */
    u32 bit2 : 1;
    u32 bit3 : 1;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 bit8 : 1;
    u32 bits9 : 5;
    u32 bit14 : 1;
    char pad28[0x08];
    int unk30;                      /* 0x30 */
    char pad34[0x14];
} GameStatus;

/* D_004325A0 (the camera, it seems). Other files still declare it as float[],
   so the extern stays in each file. */
typedef struct {
    char pad00[0x78];
    f32 unk78;                      /* 0x78 set with unk7C by func_00195878 */
    s32 unk7C;                      /* 0x7C */
} CameraState;

/* one of the 0x24-byte slots at D_005C5940, linked in a ring. D_004471E4 is the
   one being shown and D_004471E8 the next free one. */
typedef struct MessageSlot {
    char *text;                     /* 0x00 */
    int unk04;                      /* 0x04 */
    int unk08;                      /* 0x08 */
    int length;                     /* 0x0C */
    struct MessageSlot *next;       /* 0x10 */
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
    int unk1C;                      /* 0x1C */
    int unk20;                      /* 0x20 */
} MessageSlot;

/* D_00420D00: the event runner */
typedef struct {
    char pad00[0x50];
    void *current;                  /* 0x50 the event being run */
} EventRunner;

/* an event func_00120130 hands to func_0011FDD0 */
typedef struct {
    char pad00[0x10];
    u32 flags;                      /* 0x10 */
} GameEvent;

/* an 8-byte wall-clock time passed by value (func_001D2390, func_001D23F0, func_001D24A8) */
typedef struct {
    char unk0;                      /* 0x00 */
    u8 sec;                         /* 0x01 */
    u8 min;                         /* 0x02 */
    u8 hour;                        /* 0x03 */
    char unk4;                      /* 0x04 */
    u8 mday;                        /* 0x05 */
    u8 mon;                         /* 0x06 */
    u8 year;                        /* 0x07 years since 2000 */
} ClockTime;

/* D_00431CE8: func_0020CD00 advances its counter once a frame */
typedef struct {
    char pad00[0x08];
    int ticks;                      /* 0x08 */
} GameClock;

/* D_00432820: what decides whether func_0020CD00 advances GameClock.ticks */
typedef struct {
    char pad00[0x213];
    char unk213;                    /* 0x213 nonzero: the clock stops */
    char unk214;                    /* 0x214 nonzero: the clock stops */
    char pad215[0x73];
    int flags;                      /* 0x288 0x200: tick every fourth frame */
} ClockControl;

/* D_01E0E9C0: the 0x800-byte text buffer func_00217EE0 appends to */
typedef struct {
    char pad00[0x4C];
    int used;                       /* 0x4C bytes of text */
    char text[0x800];               /* 0x50 */
} TextLog;

/* one of the three 0x180-byte records at *D_0044733C, cleared by func_001FBAC8 */
typedef struct {
    char flag0;                     /* 0x00 */
    char flag1;                     /* 0x01 */
    char pad02[0x02];
    int words[10];                  /* 0x04 */
    char pad2C[0x14C];
    int tail[2];                    /* 0x178 */
} ScratchSlot;

/* D_0042A8F0[32]: a pool of 0x70-byte slots chained into a free list through `next`;
   D_00446DF0 holds the head */
typedef struct PoolSlot {
    char pad00[0x50];
    struct PoolSlot *next;          /* 0x50 */
    char pad54[0x1C];
} PoolSlot;

#endif
