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

/* Steps the five link nodes one item back: the fourth node's model moves into the first, the others are reset,
   and the nodes are chained again in their new order. */
void func_001E4DC8(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_00426560) {
        ((void (*)(s32))func_001D3730)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))func_001D3650)(value); // match: as above
    }
    func_001D45B8((MenuFreeRequest *)&D_00426470);
    D_00426470 = D_00426560;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004264C0.next = NULL;
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_00426510.next = NULL;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    func_001D4598(&D_00426560);
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;    if (pending == NULL) {
        pending = &D_00426510;
    }
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
}
