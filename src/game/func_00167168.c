#include "kf_entity.h"

extern f32 D_00431AF0[16];

float func_001957B8(float, float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_0018BD18(void *, void *, void *);
void func_0018C4B8(void *, void *);
void func_00216B48(int, int *, int);
float func_00195388(float *, float, float);

void func_00167168(Entity *entity, s32 mode) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *bone = &((EntityModelPart80 *)entity->model->unkF8)[3];
    f32 parent[16];
    f32 rotation[16];
    f32 twist[16];
    f32 angles[4];
    f32 yaw;
    f32 rate;

    if (bone != NULL) {
        if (mode != 0) {
            yaw = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (yaw > 0.6981317f) {
                yaw = 0.6981317f;
            }
            if (yaw < 0.0f) {
                yaw = 0.0f;
            }
            rate = 10.0f;
            func_00195388(&unkE0->unk48, yaw, rate);
            sceVu0CopyMatrix(parent, D_00431AF0);
            func_0018BD18(entity->model->unkF8, bone, parent);
            parent[12] = 0.0f;
            parent[13] = 0.0f;
            parent[14] = 0.0f;
            parent[15] = 1.0f;
            sceVu0RotMatrixX(rotation, D_00431AF0, bone->unk50[0]);
            sceVu0RotMatrixZ(rotation, rotation, bone->unk50[2]);
            sceVu0RotMatrixY(rotation, rotation, bone->unk50[1]);
            func_00216B48((int)rotation, (int *)parent, (int)rotation);
            sceVu0RotMatrixY(twist, D_00431AF0, unkE0->unk48);
            func_00216B48((int)rotation, (int *)twist, (int)rotation);
            sceVu0TransposeMatrix(parent, parent);
            func_00216B48((int)rotation, (int *)parent, (int)rotation);
            func_0018C4B8(angles, rotation);
            func_00195388(&unkE0->unk4C, angles[0], rate);
            func_00195388(&unkE0->unk50, angles[1], rate);
            func_00195388(&unkE0->unk54, angles[2], rate);
            bone->unk50[0] = unkE0->unk4C;
            bone->unk50[1] = unkE0->unk50;
            bone->unk50[2] = unkE0->unk54;
        } else {
            bone->unk50[0] = unkE0->unk4C;
            bone->unk50[1] = unkE0->unk50;
            bone->unk50[2] = unkE0->unk54;
        }
    }
}
