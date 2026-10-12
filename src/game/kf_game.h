/* Game-wide state blocks. */
#ifndef KF_GAME_H
#define KF_GAME_H

#include "common.h"

/* D_0038CC50 (cleared as 0x48 bytes by func_001127F8). Other files still
   declare it as char[]/int[], so the extern stays in each file. */
typedef struct {
    char unk00;                     /* 0x00 nonzero: script blends are skipped */
    char unk01;                     /* 0x01 */
    char paused;                    /* 0x02 1 while func_00101F90 holds the game */
    s8 unk03;                       /* 0x03 */
    u8 unk04;                       /* 0x04 0 or 1 */
    s8 unk05;                       /* 0x05 nonzero: the camera moves to CameraState.unk30 */
    char pad06;
    char unk07;                     /* 0x07 */
    char pad08[0x18];
    int unk20;                      /* 0x20 index into D_0041A6F8 (TriggerTypeDef), used with bit6 */
    u32 bit0 : 1;                   /* 0x24 */
    u32 bit1 : 1;                   /* set while func_0011FDD0 runs an event */
    u32 bit2 : 1;
    u32 bit3 : 1;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 bit8 : 1;
    u32 bit9 : 1;                   /* set to leave the main loop (func_001001C0) */
    u32 bits10 : 2;
    u32 bit12 : 1;                  /* bits 12 and 13 both set: the main loop skips drawing and input */
    u32 bit13 : 1;
    u32 bit14 : 1;
    int unk28;                      /* 0x28 */
    int unk2C;                      /* 0x2C */
    int unk30;                      /* 0x30 */
    int unk34;                      /* 0x34 set with bit5 by func_001226E0 */
    char pad38[0x10];
} GameStatus;

/* D_00446A28: the play time as func_001D22B8 splits it (byte 1 seconds, 2 minutes, 3 and 5 hours), passed by value */
typedef struct {
    char bytes[8];
} PlayTime;

/* D_004325A0 (the camera, it seems). Other files still declare it as float[],
   so the extern stays in each file. */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 unk10;                      /* 0x10 rotation about x */
    f32 unk14;                      /* 0x14 rotation about y */
    f32 unk18;                      /* 0x18 rotation about z */
    char pad1C[0x04];
    f32 unk20[4];                   /* 0x20 the point to look at (GameStatus.unk04) */
    f32 unk30[4];                   /* 0x30 the point to move to (GameStatus.unk05) */
    f32 unk40;                      /* 0x40 how fast it turns to unk20 */
    f32 unk44;                      /* 0x44 how far it moves to unk30 in a frame */
    char pad48[0x04];
    f32 unk4C;                      /* 0x4C added to the height above the player */
    f32 unk50[4];                   /* 0x50 the first shake's offset (func_00100D10) */
    f32 unk60[4];                   /* 0x60 the second shake's offset */
    f32 unk70;                      /* 0x70 how far the first shake reaches */
    s32 unk74;                      /* 0x74 frames of it left */
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
    f32 unk00[4];                   /* 0x00 a position the camera is put in front of (func_00100FC8) */
    f32 unk10[4];                   /* 0x10 a direction from it */
    char pad20[0x20];
    f32 unk40;                      /* 0x40 how far along unk10 */
    char pad44[0x0C];
    void *current;                  /* 0x50 the event being run */
} EventRunner;

/* an event func_00120130 hands to func_0011FDD0 */
typedef struct {
    char pad00[0x10];
    u32 flags;                      /* 0x10 */
} GameEvent;

/* the viewers func_0011A500 runs, indexed by picked_viewer (kf_map_viewer.c), 0xC bytes each */
typedef struct {
    void (*run)(void);              /* 0x00 loops until the viewer is left */
    int unk04;                      /* 0x04 */
    int count;                      /* 0x08 the number of pictures; viewer_picture stays below it */
} ViewerEntry;

/* four texture coordinates, one per corner of the viewer's player marker: a local table func_0011AAE8
   copies whole from read-only data (D_0043E760, D_0043E770) */
typedef struct {
    s32 values[4];                  /* 0x00 */
} ViewerCornerTable;

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

/* D_004147F8[380]: 0x40-byte definitions read from a data file by func_00112648
   (150, then 70, then 160 of them: see func_001029A8) */
typedef struct {
    char pad00[0x33];
    u8 unk33;                       /* 0x33 */
    char pad34[0x08];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x01];
    u16 unk3E;                      /* 0x3E unk33 scaled by D_0043E568, worked out after loading */
} GameDefinition;

/* a GameDefinition as the save code reads it (func_00103C18, func_00104EC8): unk33 is the top byte of flags */
typedef struct {
    char pad00[0x30];
    u32 flags;                      /* 0x30 bits 18 and 19 are a state; below 2 it is saved as one bit */
    char pad34[0x0C];
} GameDefinitionFlags;

/* D_00369830 and the five tables after it: one byte per GameDefinition */
typedef struct {
    u8 bytes[380];
} DefinitionByteSet;

/* D_0034C350[50]: the 50 flags of one area, a bit each */
typedef struct {
    u8 bits[8];
} AreaFlagBits;

/* a word read from any alignment (the save image header) */
typedef struct {
    int value;
} __attribute__((packed)) UnalignedInt;

/* D_0043E460: the x offsets func_001067E0 cycles its sprites through */
typedef struct {
    int x[3];
} LayerOffsets;

/* D_00467680, D_00467940, D_00467C00: an 8 x 11 grid of points func_00108020 wobbles */
typedef struct {
    int points[8][11][2];
} __attribute__((aligned(16))) RippleGrid;

/* a node of the doubly linked list at D_004469D4 */
typedef struct ListNode {
    char pad00[0x20];
    struct ListNode *prev;          /* 0x20 */
    struct ListNode *next;          /* 0x24 */
} ListNode;

/* what D_004471E4 and D_004471E8 point at; func_001CFC80 clears unk04 */
typedef struct {
    char pad00[0x04];
    int unk04;                      /* 0x04 */
} PendingSlot;

/* D_0042A8F0[32]: a pool of 0x70-byte slots chained into a free list through `next`;
   D_00446DF0 holds the head */
typedef struct PoolSlot {
    char pad00[0x50];
    struct PoolSlot *next;          /* 0x50 */
    char pad54[0x1C];
} PoolSlot;

#endif
