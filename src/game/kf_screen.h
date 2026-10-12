/* Screen effects: fades, alpha-blended overlays and the screen wobble table. */
#ifndef KF_SCREEN_H
#define KF_SCREEN_H

#include "common.h"
#include "kf_model.h"

/* the model func_0010AEB8 draws; the older name for ModelInstance */
typedef ModelInstance OverlaySprite;

/* one 0x70-byte element func_002020A0 draws (func_00202EB0, func_00202F50 and
   func_00202FD0 run it over an array of them) */
typedef struct {
    Vector4 rows[4];                /* 0x00 a matrix, loaded whole */
    char pad40[0x30];
} RenderSlot;

/* func_002020A0's second argument: eight bytes passed in one register, only spilled */
typedef struct {
    s32 unk00;                      /* 0x00 */
    s32 unk04;                      /* 0x04 */
} RenderSlotArg;

/* D_0038D680: the screen fade */
typedef struct {
    char pad00[0x28];
    int timer;                      /* 0x28 frames left */
} ScreenFade;

/* D_00471DE0[16]: the screen wobble, one sine wave per entry */
typedef struct {
    f32 amp_x;                      /* 0x00 */
    f32 amp_y;                      /* 0x04 */
    s16 x;                          /* 0x08 amp_x * sin(phase), 4.12 fixed point */
    s16 y;                          /* 0x0A */
    f32 phase;                      /* 0x0C */
} WaveOscillator;

/* what D_00446978 points at */
typedef struct {
    char pad00[0x82];
    u16 unk82;                      /* 0x82 */
} DisplaySettings;

typedef struct {
    char pad00[0xFC];
    DisplaySettings *settings;      /* 0xFC */
} DisplayContext;

/* the nodes func_001157D8 sorts into the lists D_00446788 and D_0044678C by box area */
typedef struct AreaNode {
    char pad00[0x3C];
    s32 flags;                      /* 0x3C bit 25 skips insertion, bit 31 picks the second list */
    f32 left;                       /* 0x40 */
    char pad44[0x04];
    f32 top;                        /* 0x48 */
    char pad4C[0x04];
    f32 right;                      /* 0x50 */
    char pad54[0x04];
    f32 bottom;                     /* 0x58 */
    struct AreaNode *next;          /* 0x5C */
} AreaNode;

#endif
