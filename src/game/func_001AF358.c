#include "kf_effect.h"

int func_001B4F90(void *effect);

/* Like func_001AF298, but returns 0, 1 or 2 by the hit flags from func_001B4F90. */
int func_001AF358(EffectActor *effect, s8 *state) {
    int hit_flags;

    if (*state == 1) {
        func_001A2A18(effect);
        *state = 0;
        return 0;
    }
    if (effect->effect_param != 0xFFFF && effect->def->unk10B != 0xFF) {
        hit_flags = func_001B4F90(effect);
        if (hit_flags & 2) {
            if (effect->def->unkF2 != 0) {
                func_001A1E28(effect->def->unkF2, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            return 1;
        }
        if (hit_flags != 0) {
            if (effect->def->end_effect != 0xFFFF) {
                func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                *state = 1;
            }
            return 2;
        }
    }
    *state = 0;
    return 0;
}
