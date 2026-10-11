#include "kf_menu.h"

extern u16 D_00446DE8;
extern MenuItem *D_005CE880[];
extern MenuItem *D_005CE890[];
extern MenuItem *D_005CE8A0[];
s32 func_0019CDC0(s32); /* tail-jumps to func_001CE690, whose result it returns */
void func_001D4F50(void);
void func_001D52D8(int);
void func_001D5460(int);

/* Steps the selected list node forward (0x10) or back (0x40). */
int func_001DFEF8(void) {
    MenuItem *cur = D_005CE880[0];
    MenuItem *n;

    if (cur != NULL) {
        if (D_00446DE8 & 0x10) {
            n = cur->next;
            if (n != NULL) {
                if (cur == D_005CE890[0] && n != D_005CE8A0[0]->prev) {
                    D_005CE890[0] = n;
                    D_005CE8A0[0] = D_005CE8A0[0]->next;
                }
                func_0019CDC0(1);
                D_005CE880[0] = cur->next;
            }
            func_001D52D8(0);
        } else if (D_00446DE8 & 0x40) {
            if (cur->prev != NULL) {
                if (cur == D_005CE8A0[0]->prev && D_005CE890[0]->next != cur) {
                    D_005CE8A0[0] = cur;
                    D_005CE890[0] = D_005CE890[0]->prev;
                }
                func_0019CDC0(1);
                D_005CE880[0] = cur->prev;
            }
            func_001D5460(0);
        } else {
            return 0;
        }
        func_001D4F50();
        return 1;
    }
    return 0;
}
