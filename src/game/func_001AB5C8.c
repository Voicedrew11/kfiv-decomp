#include "common.h"
#include "kf_effect.h"

void func_00216778(void *, void *);
int rand(void);

/* Starts a tilting, falling effect at its model's position, with a random fall acceleration around the definition's gravity. */
void func_001AB5C8(EffectActor *effect, EffectSlot *spawn_slot) {
    TiltFallMotion *motion;
    f32 random_fraction;
    f32 gravity;

    motion = (TiltFallMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_00216778(&motion->velocity, &effect->model->position);
    motion->tilting_back = 0;
    motion->tilt_accel = 0.0f;
    motion->fall_speed = 0.0f;
    random_fraction = (rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f;
    gravity = effect->def->gravity;
    motion->tilt_speed = 0.0f;
    motion->fall_accel = random_fraction * 0.005f + gravity;
}
