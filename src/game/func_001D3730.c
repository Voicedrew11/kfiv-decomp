#include "kf_link.h"

void func_001D40C0(LinkNode *);

extern LinkNode D_005CE8B0;
extern LinkNode *D_00446C90;

/* Copy the current upload node into the spare node D_005CE8B0 and cut the chain after it. */
void func_001D3730(void) {
    LinkNode *node = D_00446C90;

    if (node != NULL) {
        if (node->callback == (void (*)(void))func_001D40C0) {
            if (node != &D_005CE8B0) {
                D_005CE8B0 = *node;
                node->unk20 = 0;
                node->unk1C = 0;
                D_005CE8B0.unk10 = node;
                D_00446C90 = &D_005CE8B0;
                if (D_005CE8B0.next != NULL && D_005CE8B0.next->callback == (void (*)(void))func_001D40C0) {
                    D_005CE8B0.next = NULL;
                }
                return;
            }
            if (node->next->callback != node->callback) { // match: next is not checked for NULL here
                return;
            }
        }
        node->next = NULL;
    }
}
