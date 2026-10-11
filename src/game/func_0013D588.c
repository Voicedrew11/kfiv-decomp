#include "kf_entity.h"


extern Qword D_0043E8B0;
extern int D_00413F80[];
float func_00194FB8(float);
int func_0018E3D0(void *, int, int, int, void *, int, float);

void func_0013D588(Entity *actor) {
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */
    Qword pair_copy;
    ModelInstance *model;
    ModelPart *joint;
    float angle;

    pair_copy = D_0043E8B0;
    model = ENTITY_MODEL(actor);
    joint = &model->parts[3];
    if (joint != NULL && actor->unkD1 == 0x11) {
        func_0018E3D0(model, (int)joint, (int)actor->unkE0.unk30, 1, D_00413F80, 0, 10.0f);
        angle = joint->unk50 - 2.0943952f;
        joint->unk50 = angle;
        joint->unk50 = func_00194FB8(angle);
    }
}
