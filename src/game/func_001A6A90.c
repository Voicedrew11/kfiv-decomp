#include "common.h"
#include "kf_effect.h"

void func_001AF298(EffectActor *effect, void *unk10);
int func_001B00A0(float, void *);
void sceVu0AddVector(void *out, void *a, void *b);

/* Moves the effect until it comes near the player or outlives radius_half frames, then ends it. */
void func_001A6A90(EffectActor *effect) {
    SpeedFlagMotion *motion = (SpeedFlagMotion *)&effect->motion;
    EffectDef *def;
    u16 max_frames;
    int expired;
    f32 unused[4]; // match: retail reserves 0x10 bytes of stack it never uses

    if ((s8)motion->flag14 != 0) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    expired = def->radius_half < effect->frame;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame != 0) {
        func_001AF298(effect, &motion->flag14);
        if (func_001B00A0(150.0f, &effect->model->position)) {
            motion->flag14 = 1;
        }
        if (expired) {
            motion->flag14 = 1;
        }
    }
    effect->frame++;
}
