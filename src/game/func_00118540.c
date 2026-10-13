#include "kf_gold.h"
#include "kf_map.h"
#include "kf_map_scroll.h"

extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;
extern int D_00446770;
extern int D_00446794;

void func_00114840(int);
void func_00114AA8(int);
void func_001154D8(int, int);
void func_001155F8(int);
void func_0011C708(int);
void func_0011D0D0(int, int);
void func_001A15F0(void);
void func_001C82C0(int);
void func_001C8990(int, f32 *);

/* Map load step: sets up the loaded map in the other slot, shifted so that it joins the current one. */
void func_00118540(void) {
    int other_slot = D_00446770 == 0;
    f32 offset[4];

    offset[0] = -D_0038D610.request->x;
    offset[1] = -D_0038D610.request->y;
    offset[2] = -D_0038D610.request->z;
    offset[3] = 0.0f;
    if (D_00446794 == 0) {
        D_0038CD20[other_slot].tag = D_0038D610.request->map;
    } else {
        D_0038CD20[other_slot].tag = D_0038D640.map;
    }
    func_00114840(other_slot);
    func_00114AA8(other_slot);
    func_001154D8(other_slot, (int)offset);
    func_001155F8(other_slot);
    KF_MapScroll_ClearPairs(other_slot);
    func_001A15F0();
    func_0011C708(other_slot);
    func_0011D0D0(other_slot, (int)offset);
    func_001C82C0(other_slot);
    func_001C8990(other_slot, offset);
    KF_Gold_InitMap(other_slot);
    KF_Gold_MoveMap(other_slot, offset);
    D_0038D610.step++;
}
