#include "common.h"
#include "kf_effect.h"

void EffectHit_CheckAndEnd(void *, void *);
void sceVu0AddVector(void *, void *, void *);
void Vector_Scale(void *, void *, float);

/* Per-frame update of an effect that speeds up along its heading while bobbing up and down. */
void func_001A7F88(EffectActor *effect) {
    BobbingGrowMotion *motion;
    Vector4 step;
    EffectDef *def;
    u16 max_frames;

    motion = (BobbingGrowMotion *)&effect->motion;
    if (motion->ended != 0) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        motion->velocity_scale += def->scale_per_frame;
        Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &step);
    }
    motion->bob_speed += motion->bob_rising != 0 ? 0.01f : -0.01f;
    if (motion->bob_limit < motion->bob_speed) {
        motion->bob_rising = FALSE;
    }
    if (motion->bob_speed < -motion->bob_limit) {
        motion->bob_rising = TRUE;
    }
    effect->model->position.y += motion->bob_speed;
    if (effect->frame >= 3 && effect->frame < 48) {
        EffectHit_CheckAndEnd(effect, &motion->ended);
        if (motion->ended != 0) {
            Effect_Spawn(0xDF, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        }
    }
    effect->frame++;
}
