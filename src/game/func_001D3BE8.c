#include "kf_link.h"
#include "kf_menu.h"

void func_001D3650(void);
void func_001D3730(void);
void func_001D45B8(MenuFreeRequest *);
void func_001D46F0(void);

extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern LinkNode *D_00446C90;

/* Rotate the upload nodes: node 0 moves to node 3, node 4 to node 0, node 4 is made fresh; then rechain them. */
void func_001D3BE8(int unused, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (D_00446C90 != &D_00426560) {
        func_001D3730();
    } else {
        func_001D3650();
    }
    func_001D45B8((MenuFreeRequest *)&D_00426560);
    D_00426560 = D_00426470;
    D_00426560.next = NULL;
    if (D_00426560.unk20 == 0) {
        pending = &D_00426560;
    }
    D_00426470 = D_004265B0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_004265B0);
    if (pending == NULL) {
        pending = &D_004265B0;
    }
    D_004264C0.next = NULL;
    D_00426510.next = NULL;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.next = NULL;
    D_004265B0.unk08 = -1;
    current = D_00446C90;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
    func_001D46F0();
}
