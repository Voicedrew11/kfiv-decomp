#include "kf_screen.h"

extern AreaNode *D_00446788;
extern AreaNode *D_0044678C;

/* Inserts the node into one of two lists, keeping each sorted by box area. */
void func_001157D8(AreaNode *node) {
    AreaNode **head;
    AreaNode *prev;
    AreaNode *cur;
    f32 area;

    if (!(node->flags & 0x02000000)) {
        head = (node->flags & 0x80000000) ? &D_0044678C : &D_00446788;
        area = (node->right - node->left) * (node->bottom - node->top);
        prev = NULL;
        cur = *head;
        while (cur != NULL && !(area < (cur->right - cur->left) * (cur->bottom - cur->top))) {
            prev = cur;
            cur = cur->next;
        }
        if (prev != NULL) {
            prev->next = node;
        } else {
            *head = node;
        }
        node->next = cur;
    }
}
