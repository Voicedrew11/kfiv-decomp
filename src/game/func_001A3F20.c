#include "common.h"
#include "kf_effect.h"

void EffectHit_CheckAndEnd(void *, void *);
void Vector_Copy(void *, void *);
void sceVu0AddVector(void *, void *, void *);
void Vector_Scale(void *, void *, float);

void func_001A3F20(EffectActor *effect) {
    ScaledFallMotion *motion;
    ModelInstance *model;
    Vector4 step;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion = (ScaledFallMotion *)&effect->motion;
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (EFFECT_DEF_FLAGS(effect->def) & 0x80000) {
            EffectAngle_ToVector(effect->def->speed, &step, &effect->model->rotation);
            motion->scale += effect->def->scale_per_frame;
            Vector_Scale(&step, &step, motion->scale);
        } else {
            if (effect->def->gravity != 0.0f) {
                motion->fall_speed += effect->def->gravity;
                motion->velocity.y += motion->fall_speed;
            }
            Vector_Copy(&step, motion);
            if (effect->def->scale_per_frame != 0.0f) {
                motion->scale += effect->def->scale_per_frame;
                step.x *= motion->scale;
                step.z *= motion->scale;
            }
        }
        model = effect->model;
        sceVu0AddVector(&model->position, &model->position, &step);
    }
    EffectHit_CheckAndEnd(effect, motion->unk1C);
    effect->frame += 1;
}
