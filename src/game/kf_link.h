/* Linked nodes: the five chained 0x50-byte nodes at D_00426470. */
#ifndef KF_LINK_H
#define KF_LINK_H

#include "common.h"

typedef struct LinkNode {
    s64 unk00;                      /* 0x00 */
    s32 unk08;                      /* 0x08 set to -1 when the node is reset */
    void *pool_node;                /* 0x0C */
    struct LinkNode *unk10;         /* 0x10 another node; func_001EEE50 clears its next too */
    struct LinkNode *next;          /* 0x14 */
    void (*callback)(void);         /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    s32 unk20;                      /* 0x20 */
    s8 slot;                        /* 0x24 which D_00420DF0 entry its file is for (func_001D4228) */
    char pad25[0x03];
    char name[0x28];                /* 0x28 file name built with sprintf */
} LinkNode;

/* the head of a file a link node loads: two parts, placed by their offsets from the start */
typedef struct {
    char pad00[0x04];
    s32 first_offset;               /* 0x04 */
    char pad08[0x04];
    s32 second_offset;              /* 0x0C */
} LinkFile;

/* D_0043F720: the second argument of func_002032E0 for each slot (the D_00420DF0 entries are PoolPartSource, kf_game.h) */
typedef struct {
    u8 value[12];
} LinkSlotValues;

int func_001D4598(LinkNode *node);

#endif
