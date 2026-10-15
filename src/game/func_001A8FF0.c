#include "common.h"
#include "kf_effect.h"

void Vector_Subtract(void *, void *, void *);
void EffectAngle_FromVector(void *, void *);
float func_001B0178(void *, void *);
f32 func_001B0950(f32);
void Vector_Copy(void *, void *);

void func_001A8FF0(EffectActor *actor, Vector4 *target) {
    ScaledMotion *motion;
    Vector4 offset;
    float distance;

    motion = (ScaledMotion *)&actor->motion;
    Vector_Subtract(&offset, target, &actor->model->position);
    EffectAngle_FromVector(&actor->model->rotation, &offset);
    distance = func_001B0178(target, &actor->model->position);
    motion->unk24 = func_001B0950((distance - actor->def->radius_half) / 500.0f);
    Vector_Copy(&motion->unk10, target);
}
