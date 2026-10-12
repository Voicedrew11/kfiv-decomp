#include "kf_link.h"

extern LinkNode *D_00446C90;

/* Run the current link node's callback, or step to the next node when it has none or is finished. */
void func_001D3608(int unused) {
    LinkNode *node = D_00446C90;

    if (node != NULL) {
        if (node->unk20 == 0 && node->callback != NULL) {
            ((void (*)(LinkNode *))node->callback)(node); // match: the callback gets its node
        } else {
            D_00446C90 = node->next;
        }
    }
}
