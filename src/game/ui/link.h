/* Linked nodes: the five chained 0x50-byte nodes at model_load_shown. */
#ifndef KF_LINK_H
#define KF_LINK_H

#include "common.h"

typedef struct LinkNode {
    s64 unk00;                      /* 0x00 */
    s32 file_handle;                /* 0x08 file handle of the file the node loads, -1 when none */
    void *pool_node;                /* 0x0C */
    struct LinkNode *target_node;   /* 0x10 the node this load is for: its model file is read and the buffer goes to it */
    struct LinkNode *next;          /* 0x14 */
    void (*callback)(void);         /* 0x18 */
    s32 stage;                      /* 0x1C which stage of the load runs next */
    s32 done;                       /* 0x20 1 once the load has finished */
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
   (the pool_part_sources entries are PoolPartSource, core/game.h) */
typedef struct {
    u8 value[12];
} LinkSlotValues;

int Menu_TakeLoadBuffer(LinkNode *node);

#endif
