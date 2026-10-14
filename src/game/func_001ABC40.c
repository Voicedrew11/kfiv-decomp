#include "common.h"
#include "kf_effect.h"

void func_001AF298(EffectActor *effect, void *unk10_w);
void Vector_Copy(void *dst, void *src);
void sceVu0AddVector(void *out, void *a, void *b);

/* Moves its model: copies its start position, aims its velocity at radius_half, then adds the velocity. */
void func_001ABC40(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 frame;

    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    frame = effect->frame;
    if (def->radius_half < frame) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    } else if (frame < def->radius_half) {
        Vector_Copy(&effect->model->position, effect->start_position);
    } else {
        func_001AF538(def->speed, &motion->velocity, effect->start_rotation);
    }
    func_001AF298(effect, &motion->unk10.w);
    effect->frame += 1;
}
