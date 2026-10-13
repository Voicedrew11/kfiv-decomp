#include "kf_effect.h"
#include "kf_linked_point.h"



void func_001AD7A0(EffectActor *effect) {
    func_001AEC48(effect);
    func_001AF0B0(effect);
    LinkedPoint_Retarget(100.0f * effect->model->unkE8, EFFECT_FLAG_PARTS(effect)->unk44, effect);
    effect->frame += 1;
}
