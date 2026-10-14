#include "kf_effect.h"
#include "kf_linked_point.h"


void Vector_Copy(void *, void *);

/* Copies the motion vector into the model position, then the target's model position into the motion. */
void func_001AD6A8(EffectActor *effect) {
    EffectMotion *motion;

    func_001AEC48(effect);
    motion = &effect->motion;
    func_001AF0B0(effect);
    LinkedPoint_Retarget(100.0f * effect->model->unkE8, EFFECT_FLAG_PARTS(effect)->unk44, effect);
    Vector_Copy(&effect->model->position, motion);
    Vector_Copy(motion, &effect->target->model->position);
    effect->frame += 1;
}
