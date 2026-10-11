/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENT_H
#define KF_ENT_H

#include "common.h"

/* what Ent.info points at; despawn ranges */
typedef struct {
    char pad00[0x04];
    f32 y;              /* 0x04 */
    char pad08[0x40];
    u16 yrange;         /* 0x48 0: no vertical limit */
    char pad4A[0x10];
    u16 range;          /* 0x5A */
} EntInfo;

/* sub-struct at Ent+0xE0 */
typedef struct {
    char pad00[0x7A];
    char unk7A;         /* 0x7A */
} EntE0;

typedef struct Ent {
    int *model;         /* 0x00 position at +0xC0 */
    char pad04[0x08];
    char *unk0C;        /* 0x0C */
    EntInfo *info;      /* 0x10 */
    char pad14[0x08];
    struct Ent *next;   /* 0x1C */
    char pad20[0x1C];
    f32 height;         /* 0x3C */
    char pad40[0x24];
    f32 dist;           /* 0x64 distance to the player */
    char pad68[0x6B];
    char unkD3;         /* 0xD3 */
    char padD4;
    char unkD5;         /* 0xD5 group, freed together by func_001231C0 */
    char padD6[0x02];
    s64 flags;          /* 0xD8 */
    EntE0 unkE0;        /* 0xE0 */
} Ent;

extern Ent *D_004467E0; /* entity list head */
extern Ent *D_004470A8; /* other lists of the same layout */
extern Ent *D_004470B0;

#endif
