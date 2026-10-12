#include "common.h"
#include "kf_effect.h"

void KF_Vector_Subtract(void *, void *, void *);
void func_001AF5B0(void *, void *);
float func_001B0178(void *, void *);
f32 func_001B0950(f32);
void func_00216778(void *, void *);

void func_001A8FF0(EffectActor *actor, Vector4 *target) {
    ScaledMotion *motion;
    Vector4 offset;
    float distance;

    motion = (ScaledMotion *)&actor->motion;
    KF_Vector_Subtract(&offset, target, &actor->model->position);
    func_001AF5B0(&actor->model->rotation, &offset);
    distance = func_001B0178(target, &actor->model->position);
    motion->unk24 = func_001B0950((distance - actor->def->radius_half) / 500.0f);
    func_00216778(&motion->unk10, target);
}
