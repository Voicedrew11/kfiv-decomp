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

/* Rotate the upload nodes the other way: node 0 moves to node 4, node 3 to node 0, node 3 is made fresh;
   then rechain them. */
void func_001D3DB0(int unused, void *unused_data) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_004265B0) {
        func_001D3730();
    } else {
        func_001D3650();
    }
    func_001D45B8((MenuFreeRequest *)&D_004265B0);
    D_004265B0 = D_00426470;
    node = &D_004265B0;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_00426560; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_00426560;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_00426560);
    if (pending == NULL) {
        pending = &D_00426560;
    }
    D_004264C0.next = NULL;
    D_00426510.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.next = NULL;
    D_00426560.unk08 = -1;
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
    D_00426470.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
    func_001D46F0();
}
