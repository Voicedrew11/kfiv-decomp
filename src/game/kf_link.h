/* Linked nodes: the five chained 0x50-byte nodes at model_load_shown. */
#ifndef KF_LINK_H
#define KF_LINK_H

#include "common.h"

typedef struct LinkNode {
    s64 unk00;                      /* 0x00 */
    s32 unk08;                      /* 0x08 set to -1 when the node is reset */
    void *pool_node;                /* 0x0C */
    struct LinkNode *unk10;         /* 0x10 another node; Smith_CutModelChain clears its next too */
    struct LinkNode *next;          /* 0x14 */
    void (*callback)(void);         /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    s32 unk20;                      /* 0x20 */
    s8 slot;                        /* 0x24 which pool_part_sources entry its file is for (Menu_FinishWornModelLoad) */
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

/* slot_pool_groups: the pool group (PoolSlot_LoadGroup's second argument) of each slot
   (the pool_part_sources entries are PoolPartSource, kf_game.h) */
typedef struct {
    u8 value[12];
} LinkSlotValues;

int Menu_TakeLoadBuffer(LinkNode *node);

#endif
