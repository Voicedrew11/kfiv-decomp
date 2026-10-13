#include "kf_effect.h"

/* Spawns the end effect and sets *state; if *state is already 1, calls KF_Effect_Free instead. */
void func_001AF468(EffectActor *effect, s8 *state) {
    if (*state == 1) {
        KF_Effect_Free(effect);
        return;
    }
    KF_Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    *state = 1;
}
