#include "common.h"
#include "kf_effect.h"

void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);
void KF_Vector_Scale(void *, void *, float);

/* Per-frame update of an effect that speeds up along its heading while bobbing up and down. */
void func_001A7F88(EffectActor *effect) {
    BobbingGrowMotion *motion;
    Vector4 step;
    EffectDef *def;
    u16 max_frames;

    motion = (BobbingGrowMotion *)&effect->motion;
    if (motion->ended != 0) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        motion->velocity_scale += def->scale_per_frame;
        KF_Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
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
        func_001AF298(effect, &motion->ended);
        if (motion->ended != 0) {
            func_001A1E28(0xDF, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        }
    }
    effect->frame++;
}
