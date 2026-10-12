/* Mode handlers: the context a per-mode handler table call receives, and the mode state record. */
#ifndef KF_STATE_H
#define KF_STATE_H

#include "common.h"

/* D_005CF340, D_005CF350: passed to the handler picked from a mode table */
typedef struct {
    int unk00;                      /* 0x00 */
    int unk04;                      /* 0x04 */
    int arg;                        /* 0x08 */
} HandlerContext;

/* D_005CF3C0: the save screen func_001F71D8 runs each frame */
typedef struct {
    char pad00[0x08];
    s32 unk08;                      /* 0x08 */
    s8 unk0C;                       /* 0x0C the selected card, 0 or 1 */
    s8 unk0D;                       /* 0x0D the yes/no cursor of a question */
    char pad0E[0x03];
    s8 unk11;                       /* 0x11 set when the screen is being left */
    s8 unk12;                       /* 0x12 what the screen shows, -1: the card list */
    s8 unk13;                       /* 0x13 */
    s8 unk14;                       /* 0x14 set while a card operation runs */
    char pad15[0x03];
    s8 unk18;                       /* 0x18 set while input is ignored */
    char pad19[0x02];
    s8 unk1B;                       /* 0x1B */
    char pad1C[0x01];
    s8 unk1D;                       /* 0x1D */
    char pad1E[0x03];
    s8 unk21;                       /* 0x21 set when a choice was confirmed */
    char pad22[0x0E];
} SaveScreen;

extern SaveScreen D_005CF3C0;

/* D_005CF3F0, D_005CF410: a mode record and its sub-state (0x20 bytes apart; copied whole) */
typedef struct {
    s32 slot;                       /* 0x00 index into D_00428B80 */
    char pad04[0x0C];
    s16 state;                      /* 0x10 selects the mode handler, 0: idle */
    u16 unk12;                      /* 0x12 */
    s8 step;                        /* 0x14 */
    s8 retries;                     /* 0x15 */
    s8 phase;                       /* 0x16 */
    s8 count;                       /* 0x17 */
    u8 unk18;                       /* 0x18 */
    char pad19[0x01];
    s8 file;                        /* 0x1A */
    s8 unk1B;                       /* 0x1B */
    char pad1C[0x04];
} ModeState;

/* D_00428B80: the 0x28-byte entries of the slot table, indexed by the mode slot */
typedef struct {
    char unk00;                     /* 0x00 */
    char unk01;                     /* 0x01 */
    char unk02;                     /* 0x02 */
    char unk03;                     /* 0x03 */
    char unk04;                     /* 0x04 */
    char unk05;                     /* 0x05 */
    char unk06;                     /* 0x06 */
    char unk07;                     /* 0x07 */
    s32 *unk08;                     /* 0x08 */
    void *unk0C;                    /* 0x0C */
    void *unk10;                    /* 0x10 */
    void *unk14;                    /* 0x14 */
    s32 unk18;                      /* 0x18 */
    char pad1C[0x04];
    u16 unk20;                      /* 0x20 read whole; written and tested a byte at a time via SLOT_RECORD_BYTES */
    char pad22[0x02];
    s16 unk24;                      /* 0x24 */
    char pad26[0x02];
} SlotRecord;

/* the two bytes of SlotRecord.unk20 */
typedef struct {
    char pad00[0x20];
    char first;                     /* 0x20 */
    char second;                    /* 0x21 */
    char pad22[0x06];
} SlotRecordBytes;

#define SLOT_RECORD_BYTES(slot) ((SlotRecordBytes *)(slot))

extern SlotRecord D_00428B80[];

/* what the D_00440B70 handlers build; func_00207478 dispatches it by kind */
typedef struct {
    char pad00[0x31];
    u8 kind;                        /* 0x31 picks the function that consumes the object */
} KindObject;

/* the 0x60-byte objects D_00446BF4 points at, drawn by func_00207478 and func_00203F28 (D_00446BFC
   is taken to point at the same kind of object: inferred from the matching offsets, not proven) */
typedef struct {
    char pad00[0x20];
    u32 pad20 : 24;
    u32 unk23 : 8;                  /* 0x23 the level its fade runs to */
    char pad24[0x1A];
    u8 unk3E;                       /* 0x3E the kind */
    s8 unk3F;                       /* 0x3F */
    char pad40[0x02];
    u8 unk42;                       /* 0x42 passed to func_001E5210 while it is drawn */
    char pad43[0x1D];
} Entry60;

/* a task that runs a one-time setup (func_00103858, func_001CF988) and ends when func_001CF980 reports done */
typedef struct {
    s32 active;                     /* 0x00 cleared once the setup is done */
    char pad04[0x04];
    s32 unk08;                      /* 0x08 */
    char pad0C[0x04];
    s32 unk10;                      /* 0x10 */
    char pad14[0x04];
    s32 unk18;                      /* 0x18 */
    char pad1C[0x68];
    s32 started;                    /* 0x84 set once the setup has been started */
} SetupTask;

#endif
