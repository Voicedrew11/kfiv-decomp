#include "kf_link.h"
#include "kf_menu.h"

void func_001D3650(int); // match: defined (void); passing the handler's argument through fixes the saved registers
void func_001D3730(int); // match: defined (void); see above
void func_001D45B8(MenuFreeRequest *);
void func_001D46F0(void);

extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern LinkNode *D_00446C90;

/* Rotate the upload nodes: node 0 moves to node 1, node 2 to node 0, nodes 2, 3 and 4 are made fresh; then rechain them. */
void func_001D39F0(int arg, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (D_00446C90 == &D_00426510 || D_00446C90 == &D_00426470) {
        func_001D3730(arg);
    } else {
        func_001D3650(arg);
    }
    func_001D45B8((MenuFreeRequest *)&D_004264C0);
    D_004264C0 = D_00426470;
    D_004264C0.next = NULL;
    if (D_004264C0.unk20 == 0) {
        pending = &D_004264C0;
    }
    D_00426470 = D_00426510;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_00426510);
    if (pending == NULL) {
        pending = &D_00426510;
    }
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426510.next = NULL;
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
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = NULL;
    func_001D46F0();
}
