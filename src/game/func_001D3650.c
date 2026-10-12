#include "kf_link.h"
#include "kf_menu.h"

void func_001D40C0(LinkNode *);
void func_001D45B8(MenuFreeRequest *);

extern LinkNode D_005CE8B0;
extern LinkNode *D_00446C90;

/* Move the current upload node into the spare node D_005CE8B0, reset the original and cut the chain after it. */
void func_001D3650(void) {
    LinkNode *node = D_00446C90;

    if (node != NULL && node->callback == (void (*)(void))func_001D40C0 && node != &D_005CE8B0) {
        D_005CE8B0 = *node;
        func_001D4598(D_00446C90);
        D_00446C90->unk08 = -1;
        D_00446C90->unk20 = 0;
        D_00446C90->unk1C = 0;
        D_00446C90 = &D_005CE8B0;
        if (D_005CE8B0.next != NULL && D_005CE8B0.next->callback == (void (*)(void))func_001D40C0) {
            D_005CE8B0.next = NULL;
        }
        func_001D45B8((MenuFreeRequest *)&D_005CE8B0);
        node = NULL; // match: a statement after the call keeps it from becoming a sibling jump
    }
}
