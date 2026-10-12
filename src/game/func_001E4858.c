#include "kf_link.h"
#include "kf_menu.h"

extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern LinkNode *D_00446C90;
void func_001D3650(void);
void func_001D3730(void);
void func_001D45B8(MenuFreeRequest *);

/* Moves the selection one item: the first node's model moves aside, a neighbour's moves into the first, the
   freed nodes are reset and the five are chained again. */
void func_001E4858(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_004264C0 || D_00446C90 == &D_00426470) {
        ((void (*)(s32))func_001D3730)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))func_001D3650)(value); // match: as above
    }
    func_001D45B8((MenuFreeRequest *)&D_00426510);
    D_00426510 = D_00426470;
    D_00426510.next = NULL;
    if (D_00426510.unk20 == 0) {
        pending = &D_00426510;
    }
    D_00426470 = D_004264C0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_004264C0);
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004264C0.next = NULL;
    if (pending == NULL) {
        pending = &D_004264C0;
    }
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = NULL;
}
