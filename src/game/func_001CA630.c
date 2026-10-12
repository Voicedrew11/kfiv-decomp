#include "kf_map.h"

extern u8 D_003699B0[];
extern u8 D_00369B30[];
extern u8 D_00369CB0[];
extern u8 D_00369E30[];
extern u8 D_00369FB0[];
extern TriggerTypeDef D_0041A6F8[];

/* Returns FALSE when one of the five tables after D_00369830 holds the item and it is one of a kind. */
int func_001CA630(int item) {
    if (D_003699B0[item] != 0 && (item >= 0x96 || ((D_0041A6F8 + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (D_00369B30[item] != 0 && (item >= 0x96 || ((D_0041A6F8 + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (D_00369CB0[item] != 0 && (item >= 0x96 || ((D_0041A6F8 + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (D_00369E30[item] != 0 && (item >= 0x96 || ((D_0041A6F8 + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (D_00369FB0[item] != 0 && (item >= 0x96 || ((D_0041A6F8 + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    return TRUE;
}
