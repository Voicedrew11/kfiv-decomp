#include "kf_effect.h"

/* Spawns the end effect and sets *state; if *state is already 1, calls func_001A2A18 instead. */
void func_001AF468(EffectActor *effect, s8 *state) {
    if (*state == 1) {
        func_001A2A18(effect);
        return;
    }
    func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    *state = 1;
}
