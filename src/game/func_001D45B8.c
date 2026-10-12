#include "common.h"
#include "kf_menu.h"

extern MenuFreeNode *D_004472B0;

/* Pushes the request's node back onto the menu free list. */
void func_001D45B8(MenuFreeRequest *request) {
    MenuFreeNode *node = request->node;
    MenuFreeNode *old_head = D_004472B0;

    D_004472B0 = node;
    node->next = old_head;
}
