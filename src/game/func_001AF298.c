#include "kf_effect.h"

int func_001B4F90(void *effect);

/* Spawns the end effect once the hit check passes; *state records that it is spawned. */
void func_001AF298(EffectActor *effect, s8 *state) {
    if (*state == 1) {
        Effect_Free(effect);
        *state = 0;
    } else if (effect->effect_param != 0xFFFF && effect->def->unk10B != 0xFF && func_001B4F90(effect) != 0) {
        if (effect->def->end_effect != 0xFFFF) {
            Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            *state = 1;
        }
    } else {
        *state = 0;
    }
}
