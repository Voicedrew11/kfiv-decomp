#include "kf_link.h"
#include "kf_menu.h"

void func_001D3650(void);
void func_001D3730(void);
void func_001D45B8(MenuFreeRequest *);

extern LinkNode D_00426470;
extern LinkNode D_00426510;
extern LinkNode D_004264C0;
extern LinkNode *D_00446C90;

/* Rotate the three upload nodes the other way: node 0 moves to node 1, node 2 to node 0, node 2 is made fresh;
   then queue what still has to load and rechain them. */
void func_001D5460(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_004264C0) {
        func_001D3730();
    } else {
        func_001D3650();
    }
    func_001D45B8((MenuFreeRequest *)&D_004264C0);
    D_004264C0 = D_00426470;
    node = &D_004264C0;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_00426510; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_00426510;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_00426510);
    if (pending == NULL) {
        pending = &D_00426510;
    }
    D_00426510.unk08 = -1;
    D_00426510.unk1C = 0;
    D_00426510.next = NULL;
    D_00426510.unk20 = 0;
    current = D_00446C90;
    if (current != NULL) {
        current->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426510;
    D_00426510.next = &D_004264C0;
    D_004264C0.next = NULL;
}
