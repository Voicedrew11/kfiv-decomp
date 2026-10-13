#include "common.h"
#include "kf_effect.h"

/* Scales the model each frame, and spawns the end effect when the frame reaches the definition's radius. */
void func_001A72F0(EffectActor *effect) {
    ScalerMotion *motion;
    ModelInstance *model;
    EffectDef *def;

    motion = (ScalerMotion *)&effect->motion;
    if ((s8)((TurningMotion *)motion)->unk24 != 0) {
        KF_Effect_Free(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    model = effect->model;
    def = effect->def;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
    if (effect->frame == def->radius_half) {
        KF_Effect_Spawn(def->end_effect, &effect->motion.unk10, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    }
    effect->frame += 1;
}
