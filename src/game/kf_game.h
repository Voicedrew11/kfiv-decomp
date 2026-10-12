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
    char unk06;                     /* 0x06 nonzero: the player cannot run (func_001B5A48) */
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
    char pad38[0x0C];
    int unk44;                      /* 0x44 counts down to 0; 1 when an item was just used (func_001BC2D0) */
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
    void *unk54;                    /* 0x54 an Entity whose spawn info can answer the use of an item (func_001BC2D0) */
} EventRunner;

/* an event func_00120130 hands to func_0011FDD0 */
typedef struct {
    char pad00[0x10];
    u32 flags;                      /* 0x10 */
} GameEvent;

/* D_0038D6B0[]: the viewers func_0011A500 runs, picked by D_00447064 (0xC bytes each) */
typedef struct {
    void (*run)(void);              /* 0x00 loops until the viewer is left */
    int unk04;                      /* 0x04 */
    int count;                      /* 0x08 D_00447068 stays below it */
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
    char pad00[0x30];
    u16 unk30;                      /* 0x30 the number in the name of its item_arm file (func_001BF898) */
    char pad32[0x01];
    u8 unk33;                       /* 0x33 */
    char pad34[0x08];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x01];
    u16 unk3E;                      /* 0x3E unk33 scaled by D_0043E568, worked out after loading */
} GameDefinition;

/* a GameDefinition as the save code reads it (func_00103C18, func_00104EC8): the word unk33 is the top byte of */
typedef struct {
    char pad00[0x30];
    u32 flags;                      /* 0x30 bits 18 and 19 are a state; below 2 it is saved as one bit */
    char pad34[0x0C];
} GameDefinitionFlags;

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

/* D_005C58E8: the message box (func_001CF988 opens it, func_001CF4D0 runs it, func_001CF708 draws it) */
typedef struct {
    s16 *start;                     /* 0x00 first glyph of the page on screen */
    s16 *cursor;                    /* 0x04 glyph being typed */
    s16 *end;                       /* 0x08 end of the page (func_001D0658) */
    int delay;                      /* 0x0C frames per glyph */
    int timer;                      /* 0x10 frames since the last glyph */
    int unk14;                      /* 0x14 */
    int waiting;                    /* 0x18 1 = page typed out, waiting for a button */
    int typed;                      /* 0x1C glyphs typed on this page (control codes not counted) */
    int closing;                    /* 0x20 1 = closing */
    int close_timer;                /* 0x24 frames left of the closing */
    int left;                       /* 0x28 */
    int top;                        /* 0x2C */
    int font;                       /* 0x30 */
    int unk34;                      /* 0x34 */
    int line_offsets[4];            /* 0x38 left edge of each line (func_001D0658) */
} MessageBox;

/* D_0042A8F0[32]: a pool of 0x70-byte slots chained into a free list through `next`;
   D_00446DF0 holds the head */
typedef struct PoolSlot {
    char pad00[0x50];
    struct PoolSlot *next;          /* 0x50 */
    char pad54[0x1C];
} PoolSlot;

/* D_00413DB8[2]: a controller as the game reads it, one per port (func_001B5590). D_00413DA8 is the copy
   of port 0 that the game tests. */
typedef struct {
    u16 held;                       /* 0x00 buttons held, the stick directions among them */
    u16 pressed;                    /* 0x02 buttons that went down this frame */
    s32 type;                       /* 0x04 what func_002179F8 says: 1 has sticks, 0xFF is no controller */
    u16 look_buttons;               /* 0x08 the right stick's up and down as the buttons 0x200 and 0x100 */
    u16 prev_look_buttons;          /* 0x0A */
    u16 move_buttons;               /* 0x0C the left stick as the direction buttons (0x80, 0x20, 0x10, 0x40) */
    u16 prev_move_buttons;          /* 0x0E */
} PadState;

/* what func_00217760 returns: the controller's own report */
typedef struct {
    char pad00[0x02];
    u16 held;                       /* 0x02 */
    u8 right_stick_x;               /* 0x04 the four axes, 0x80 at rest */
    u8 right_stick_y;               /* 0x05 */
    u8 left_stick_x;                /* 0x06 */
    u8 left_stick_y;                /* 0x07 */
    char pad08[0x18];
    u16 pressed;                    /* 0x20 */
} PadReport;

/* PadReport's axis bytes two at a time: zero when the controller sends none */
typedef struct {
    char pad00[0x04];
    u16 right_stick;                /* 0x04 */
    u16 left_stick;                 /* 0x06 */
} PadReportAxes;

/* the texts of the debug start menu (func_001B0A78), local arrays it copies whole from
   read-only data: blank lines above the text, the PUSH START line, the four menu lines */
typedef struct {
    char text[0x1A];
} DebugStartMenuSpacer;

typedef struct {
    char text[0x46];
} DebugStartMenuPrompt;

typedef struct {
    char text[4][0x80];
} DebugStartMenuItems;

#endif
