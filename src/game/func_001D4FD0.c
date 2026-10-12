#include "kf_link.h"

extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern LinkNode *D_00446C90;
void func_001D40C0(LinkNode *);
void func_001D4F50(void);

/* Builds two node lists: 470 -> 4C0 -> 510 and 560 -> 5B0. */
void func_001D4FD0(void) {
    func_001D4F50();
    func_001D4598(&D_00426470);
    func_001D4598(&D_004264C0);
    func_001D4598(&D_00426510);
    func_001D4598(&D_00426560);
    func_001D4598(&D_004265B0);
    D_00426470.next = &D_004264C0;
    D_00426470.callback = func_001D40C0;
    D_004264C0.next = &D_00426510;
    D_004264C0.callback = func_001D40C0;
    D_00426510.next = NULL;
    D_00426510.callback = func_001D40C0;
    D_00426560.next = &D_004265B0;
    D_00426560.callback = func_001D40C0;
    D_004265B0.next = NULL;
    D_004265B0.callback = func_001D40C0;
    D_00426470.unk20 = 0;
    D_004264C0.unk20 = 0;
    D_00426510.unk20 = 0;
    D_00426560.unk20 = 0;
    D_004265B0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_00426510.unk1C = 0;
    D_00426560.unk1C = 0;
    D_00426470.unk1C = D_004265B0.unk1C = 0;
    D_00446C90 = &D_00426470;
}
