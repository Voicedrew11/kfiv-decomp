#include "common.h"
#include "kf_effect.h"

int func_001B4F90(void *);
void sceVu0AddVector(void *, void *, void *);
void func_001AF5B0(void *, void *);

#define SCRATCHPAD_WORD_18 (*(int *)0x70000018)

// Falls under gravity and spawns its landing effects on the first hit.
void func_001A4E38(EffectActor *effect) {
    LandedFlagMotion *motion = (LandedFlagMotion *)&effect->motion;
    int spawn_id;

    if (motion->landed) {
        KF_Effect_Free(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->gravity != 0.0f) {
            motion->velocity.y += effect->def->gravity;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        func_001AF5B0(&effect->model->rotation, motion);
    }
    if (func_001B4F90(effect)) {
        spawn_id = SCRATCHPAD_WORD_18 == 5 ? 0x16 : effect->def->end_effect;
        KF_Effect_Spawn(spawn_id, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        KF_Effect_Spawn(0x122, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->landed = 1;
    }
    effect->frame += 1;
}
