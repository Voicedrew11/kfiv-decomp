#include "kf_entity.h"

extern PlayerData D_00413F80;
extern f32 D_00431AF0[16];

double atan2(double, double);
float func_00194FB8(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_0018BD18(void *, void *, void *);
void func_0018C4B8(void *, void *);
void func_00216B48(int, int *, int);
float func_00195388(float *, float, float);

void func_0016ABB8(Entity *entity, s32 mode) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelUnkF8 *bone = &entity->model->unkF8[2];
    f32 parent[16];
    f32 rotation[16];
    f32 tilt[16];
    f32 angles[4];
    f32 pitch;
    f32 rate;

    if (bone != NULL) {
        pitch = func_00194FB8(atan2(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player));
        if (pitch > 1.0471976f) {
            pitch = 1.0471976f;
        } else if (pitch < -1.0471976f) {
            pitch = -1.0471976f;
        }
        if (mode > 0) {
            rate = 10.0f;
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
            sceVu0RotMatrixX(tilt, D_00431AF0, pitch);
            func_00216B48((int)rotation, (int *)tilt, (int)rotation);
            sceVu0TransposeMatrix(parent, parent);
            func_00216B48((int)rotation, (int *)parent, (int)rotation);
            func_0018C4B8(angles, rotation);
            func_00195388(&unkE0->unk48[0], angles[0], rate);
            func_00195388(&unkE0->unk48[1], angles[1], rate);
            func_00195388(&unkE0->unk48[2], angles[2], rate);
            bone->unk50[0] = unkE0->unk48[0];
            bone->unk50[1] = unkE0->unk48[1];
            bone->unk50[2] = unkE0->unk48[2];
        } else if (mode == 0) {
            rate = 10.0f;
            func_00195388(&unkE0->unk48[0], bone->unk50[0], rate);
            func_00195388(&unkE0->unk48[1], bone->unk50[1], rate);
            func_00195388(&unkE0->unk48[2], bone->unk50[2], rate);
            bone->unk50[0] = unkE0->unk48[0];
            bone->unk50[1] = unkE0->unk48[1];
            bone->unk50[2] = unkE0->unk48[2];
        } else {
            unkE0->unk48[0] = bone->unk50[0];
            unkE0->unk48[1] = bone->unk50[1];
            unkE0->unk48[2] = bone->unk50[2];
        }
    }
}
