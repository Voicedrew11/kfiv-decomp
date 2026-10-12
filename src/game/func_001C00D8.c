#include "kf_player.h"

void func_001C08F0(void);
void func_001C0988(void *);
void sceVu0MulMatrix(void *, void *, void *);
void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);
extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern f32 D_004323E0[16];
extern f32 D_00431AF0[16];
extern ModelInstance D_0057B6A0;

/* each frame: ages the weapon trail and queues the two ends of the held weapon, in screen space */
void func_001C00D8(void) {
    f32 world_screen[16];
    f32 unused[4];
    ModelScreenPoint point;
    ModelInstance *weapon;
    void *blade_base;
    void *blade_tip;
    s32 item;

    func_001C08F0();
    if (D_00413F80.unk258 != 0) {
        weapon = &D_0057B6A0;
        weapon->unk1B4(weapon);
        if (D_0041B9B8[D_00413F80.unk3C].unk63 == 2) {
            return;
        }
        item = D_00413F80.unk3C + 0x96;
        if (item == 0x97) {
            return;
        }
        if (item == 0x98) {
            return;
        }
        blade_base = (void *)weapon->get_point(weapon, 0);
        blade_tip = (void *)weapon->get_point(weapon, 1);
        sceVu0MulMatrix(world_screen, D_004323E0, D_00431AF0);
        KF_Vu0_LoadMatrix(world_screen);
        KF_Vu0_ProjectToScreen((int *)&point, blade_base);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.unk0C |= 0x8000;
        } else {
            point.unk0C = 0;
        }
        func_001C0988(&point);
        KF_Vu0_ProjectToScreen((int *)&point, blade_tip);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.unk0C |= 0x8000;
        } else {
            point.unk0C = 0;
        }
        func_001C0988(&point);
    }
}
