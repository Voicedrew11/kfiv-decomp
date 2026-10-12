#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern MapSlotPair D_00471FE0[2][16];

/* Carries pairs over from one map slot to the other for the maps that share them. */
void func_00117AA8(int from_slot, int to_slot) {
    int from_tag = D_0038CD20[from_slot].tag;
    int to_tag = D_0038CD20[to_slot].tag;

    if ((from_tag == 4 && to_tag == 0x63) || (from_tag == 0x63 && to_tag == 4) ||
        (from_tag == 0x62 && to_tag == 0x63) || (from_tag == 0x63 && to_tag == 0x62)) {
        D_00471FE0[to_slot][0] = D_00471FE0[from_slot][0];
        D_00471FE0[to_slot][1] = D_00471FE0[from_slot][1];
        D_00471FE0[to_slot][2] = D_00471FE0[from_slot][2];
    // match: the same copy again in its own branch; one test for all six pairs orders the code differently
    } else if ((from_tag == 0x15 && to_tag == 0x1B) || (from_tag == 0x1B && to_tag == 0x15)) {
        D_00471FE0[to_slot][0] = D_00471FE0[from_slot][0];
        D_00471FE0[to_slot][1] = D_00471FE0[from_slot][1];
        D_00471FE0[to_slot][2] = D_00471FE0[from_slot][2];
    } else if (from_tag == 0x1A && to_tag == 0x1B) {
        D_00471FE0[to_slot][4] = D_00471FE0[from_slot][5];
        D_00471FE0[to_slot][5] = D_00471FE0[from_slot][6];
        D_00471FE0[to_slot][6] = D_00471FE0[from_slot][7];
        D_00471FE0[to_slot][7] = D_00471FE0[from_slot][8];
    } else if (from_tag == 0x1B && to_tag == 0x1A) {
        D_00471FE0[to_slot][5] = D_00471FE0[from_slot][4];
        D_00471FE0[to_slot][6] = D_00471FE0[from_slot][5];
        D_00471FE0[to_slot][7] = D_00471FE0[from_slot][6];
        D_00471FE0[to_slot][8] = D_00471FE0[from_slot][7];
    }
}
