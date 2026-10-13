#include "kf_effect.h"

void func_001A7A48(EffectActor *effect);

/* Starts the effect's ending: spawns the end effect, then flags the motion as ended. */
void func_001A7AC8(EffectActor *effect) {
    BobMotion *motion;
    ModelInstance *model;

    motion = (BobMotion *)&effect->motion;
    model = effect->model;
    KF_Effect_Spawn(effect->def->end_effect, &model->position, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    func_001A7A48(effect);
    motion->ended = 1;
}
