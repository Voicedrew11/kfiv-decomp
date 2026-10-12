/* Menus: the doubly linked item lists the cursor walks (D_005CE880...). */
#ifndef KF_MENU_H
#define KF_MENU_H

#include "common.h"

typedef struct MenuItem {
    int unk00;                      /* 0x00 */
    struct MenuItem *next;          /* 0x04 */
    struct MenuItem *prev;          /* 0x08 */
    s16 unk0C;                      /* 0x0C the number in its model's file name */
} MenuItem;

#endif
