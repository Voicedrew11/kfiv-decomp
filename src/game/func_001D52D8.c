#include "kf_link.h"
#include "kf_menu.h"

void func_001D3650(void);
void func_001D3730(void);
void func_001D45B8(MenuFreeRequest *);

extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode *D_00446C90;

/* Rotate the three upload nodes: node 0 moves to node 2, node 1 to node 0, node 1 is made fresh;
   then queue what still has to load and rechain them. */
void func_001D52D8(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_00426510) {
        func_001D3730();
    } else {
        func_001D3650();
    }
    func_001D45B8((MenuFreeRequest *)&D_00426510);
    D_00426510 = D_00426470;
    node = &D_00426510;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_004264C0; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_004264C0;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    func_001D4598(&D_004264C0);
    if (pending == NULL) {
        pending = &D_004264C0;
    }
    D_004264C0.unk08 = -1;
    D_004264C0.unk1C = 0;
    D_004264C0.next = NULL;
    D_004264C0.unk20 = 0;
    current = D_00446C90;
    if (current != NULL) {
        current->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
}
