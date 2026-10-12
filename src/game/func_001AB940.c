#include "common.h"
#include "kf_effect.h"

/* Spawner: each frame, spawns effect def->unkF0 from the model until spawn_count passes the frame. */
void func_001AB940(EffectActor *effect) {
    SpawnCountMotion *motion = (SpawnCountMotion *)&effect->motion;

    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (motion->spawn_count < effect->frame) {
        func_001A1E28(effect->def->unkF0, &effect->model->position, &effect->model->rotation,
                      effect->owner_entity, effect->effect_param, effect->owner);
        func_001A2A18(effect);
    }
    effect->frame += 1;
}
