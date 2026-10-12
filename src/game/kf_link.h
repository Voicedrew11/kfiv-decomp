/* Linked nodes: the five chained 0x50-byte nodes at D_00426470. */
#ifndef KF_LINK_H
#define KF_LINK_H

#include "common.h"

typedef struct LinkNode {
    char pad00[0x0C];
    void *pool_node;                /* 0x0C */
    char pad10[0x04];
    struct LinkNode *next;          /* 0x14 */
    void (*callback)(void);         /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    s32 unk20;                      /* 0x20 */
    char pad24[0x2C];
} LinkNode;

int func_001D4598(LinkNode *node);

#endif
