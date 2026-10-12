/* Menus: the doubly linked item lists the cursor walks (D_005CE880...). */
#ifndef KF_MENU_H
#define KF_MENU_H

#include "common.h"

typedef struct MenuItem {
    int unk00;                      /* 0x00 */
    struct MenuItem *next;          /* 0x04 */
    struct MenuItem *prev;          /* 0x08 */
    s16 unk0C;                      /* 0x0C a value func_001D46B8 prints */
} MenuItem;

/* a node of the menu free list D_004472B0 (15 of them at D_005CE900) */
typedef struct MenuFreeNode {
    struct MenuFreeNode *next;      /* 0x00 */
    int unk04;                      /* 0x04 */
} MenuFreeNode;

/* what func_001D45B8 is handed: the node to give back at 0x0C */
typedef struct {
    char pad00[0x0C];
    MenuFreeNode *node;             /* 0x0C */
} MenuFreeRequest;

#endif
