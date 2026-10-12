#include "kf_link.h"
#include "kf_menu.h"

extern MenuItem *D_005CF3B0[];
extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern LinkNode *D_00446C90;
void func_001D40C0(LinkNode *);
void func_001EF648(void);

/* Rebuilds the five link nodes for the current D_005CF3B0 item and chains them (the func_001E4568 of this list). */
void func_001EEE80(void) {
    if (D_005CF3B0[0] != NULL) {
        func_001EF648();
        func_001D4598(&D_00426470);
        func_001D4598(&D_004264C0);
        func_001D4598(&D_00426510);
        func_001D4598(&D_00426560);
        func_001D4598(&D_004265B0);
        D_00426470.next = &D_004264C0;
        D_00426470.callback = (void (*)(void))func_001D40C0;
        D_00426470.unk20 = 0;
        D_004264C0.next = &D_00426510;
        D_004264C0.callback = (void (*)(void))func_001D40C0;
        D_004264C0.unk20 = 0;
        D_004264C0.unk1C = 0;
        D_00426510.next = &D_00426560;
        D_00426510.callback = (void (*)(void))func_001D40C0;
        D_00426510.unk20 = 0;
        D_00426510.unk1C = 0;
        D_00426560.next = &D_004265B0;
        D_00426560.callback = (void (*)(void))func_001D40C0;
        D_00426560.unk20 = 0;
        D_00426560.unk1C = 0;
        D_004265B0.next = NULL;
        D_004265B0.callback = (void (*)(void))func_001D40C0;
        D_004265B0.unk20 = 0;
        D_00426470.unk1C = D_004265B0.unk1C = 0;
        D_00446C90 = &D_00426470;
    }
}
