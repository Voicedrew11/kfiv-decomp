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

/* D_005CF410: the current mode and its sub-state */
typedef struct {
    char pad00[0x10];
    s16 state;                      /* 0x10 selects the mode handler, 0: idle */
    char pad12[0x02];
    char unk14;                     /* 0x14 */
    char unk15;                     /* 0x15 */
} ModeState;

/* what the D_00440B70 handlers build; func_00207478 dispatches it by kind */
typedef struct {
    char pad00[0x31];
    u8 kind;                        /* 0x31 picks the function that consumes the object */
} KindObject;

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
