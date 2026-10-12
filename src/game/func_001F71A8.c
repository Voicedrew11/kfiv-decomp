#include "common.h"
#include "kf_link.h"

extern LinkNode *D_00446C90;
extern int D_00446CFC;

int func_001F71A8(void) {
    LinkNode *head;
    LinkNode *child;

    head = D_00446C90;
    if (head == NULL) {
        D_00446CFC = 0;
        return 0;
    }
    child = head->unk10;
    head->next = NULL;
    if (child != NULL) {
        child->next = NULL;
    }
    return 1;
}
