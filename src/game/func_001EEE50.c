#include "kf_link.h"

extern LinkNode *D_00446C90;
extern int D_00446CC8;

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets D_00446CC8. */
int func_001EEE50(void) {
    LinkNode *node = D_00446C90;
    LinkNode *other;

    if (node == NULL) {
        D_00446CC8 = 0;
        return 0;
    }
    other = node->unk10;
    node->next = NULL;
    if (other != NULL) {
        other->next = NULL;
    }
    return 1;
}
