#include "kf_effect.h"

void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* At frame == def->radius_half, adds the velocity to the model position scale_per_mille / 100 times, each time calling func_001AF298; then counts a frame. */
void func_001AC0E8(EffectActor *effect) {
    ScalerMotion *motion = (ScalerMotion *)&effect->motion;
    s32 i;
    s32 steps;
    u8 flag;

    if (effect->frame == effect->def->radius_half) {
        steps = motion->scale_per_mille / 100.0f;
        for (i = 0; i < steps; i++) {
            flag = 0;
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
            func_001AF298(effect, &flag);
        }
    }
    effect->frame += 1;
}
