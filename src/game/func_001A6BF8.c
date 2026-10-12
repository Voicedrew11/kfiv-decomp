#include "common.h"
#include "kf_effect.h"

void func_00216778(void *, void *);
int func_001C0E40(f32, void *);

/* Aims a motion at its target if the target is within reach; returns 1 if it is. */
int func_001A6BF8(EffectActor *effect, float *position, AimMotion *motion) {
    Vector4 local;
    ModelInstance *target_model;
    float target_z;

    func_00216778(&local, position);
    local.y += -300.0f;
    if (func_001C0E40(1000.0f, &local.x)) {
        target_model = effect->target->model;
        motion->target_x = target_model->position.x;
        motion->unk14 = *(float *)0x70000004 - 600.0f;
        target_z = target_model->position.z;
        motion->flag25 = 1;
        motion->target_z = target_z;
        return 1;
    }
    return 0;
}
