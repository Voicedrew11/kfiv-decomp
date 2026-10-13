#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern f32 D_00431AF0[16];

void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);

/* a point `distance` away from the player, along the player's unk1AC vector turned by the heading */
void func_0018BC78(f32 *point, s32 distance) {
    f32 offset[4];

    offset[0] = D_00413F80.unk1B0;
    offset[1] = D_00413F80.unk1AC;
    offset[2] = D_00413F80.unk1B4;
    offset[3] = 0.0f;
    sceVu0RotMatrixY((f32 *)0x70000000, D_00431AF0, D_00413F80.rotation.y);
    sceVu0ApplyMatrix(offset, (int *)0x70000000, offset);
    Vector_Scale(offset, offset, distance);
    func_00216C48(point, &D_00413F80, offset);
}
