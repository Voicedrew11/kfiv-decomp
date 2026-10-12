#include "kf_link.h"
#include "kf_menu.h"

extern s8 D_00446C94;
extern LinkNode *D_00446C90;
extern s32 D_00446AC8;
extern int D_00446AD0;
extern s8 D_004472CD;
extern MenuItem *D_005CE6B0[];
extern MenuItem *D_005CE6C0[];
extern MenuItem *D_005CE6D0[];
extern MenuItem *D_005CE6E0[];
extern MenuItem *D_005CE6F0[];
extern MenuItem *D_005CE700[];
extern MenuItem *D_005CE710[];
extern MenuItem *D_005CE720[];
extern MenuItem *D_005CE730[];
extern MenuItem *D_005CE740[];
extern MenuItem *D_005CE770[];
extern MenuItem *D_005CE780[];
extern MenuItem *D_005CE790[];
extern MenuItem *D_005CE7A0[];
extern MenuItem *D_005CE7B0[];
extern MenuItem *D_005CE7C0[];
extern MenuItem *D_005CE7D0[];
extern MenuItem *D_005CE7E0[];
extern MenuItem *D_005CE7F0[];
extern MenuItem *D_005CE800[];
void func_001A1260(int);
int func_001024B0(int);

/* Step of the menu start-up: once the load chain is done, remembers the ten list heads and goes back to step 0. */
s32 func_001D7AD8(void) {
    LinkNode *node;

    switch (D_00446C94) {
    case 0:
        node = D_00446C90;
        if (node == NULL) {
            D_005CE770[0] = D_005CE6B0[0];
            D_005CE780[0] = D_005CE6C0[0];
            D_005CE800[0] = D_005CE740[0];
            D_005CE790[0] = D_005CE6D0[0];
            D_005CE7A0[0] = D_005CE6E0[0];
            D_005CE7B0[0] = D_005CE6F0[0];
            D_005CE7C0[0] = D_005CE700[0];
            D_005CE7D0[0] = D_005CE710[0];
            D_005CE7E0[0] = D_005CE720[0];
            D_005CE7F0[0] = D_005CE730[0];
            func_001A1260(D_005CE740[0]->unk0C);
            func_001024B0(D_00446AD0);
            D_00446AC8 = 0;
            D_004472CD = 0;
            return FALSE;
        }
        node->next = NULL;
        if (node->unk10 != NULL) {
            node->unk10->next = NULL;
        }
        return TRUE;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        D_00446AC8 = 0;
        return FALSE;
    }
    // match: other steps return nothing (see the report's retail bugs)
}
