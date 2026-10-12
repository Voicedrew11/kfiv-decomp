#include "kf_effect.h"

void func_00216778(void *, void *);
int rand(void);

/* Starts a spray effect with random velocity and spin around the definition's ranges. */
void func_001AABA0(EffectActor *effect, EffectSlot *spawn_slot) {
    SprayMotion *motion;

    motion = (SprayMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    func_00216778(&effect->model->rotation, effect->unk68);
    motion->velocity.x = effect->def->offset_base[0] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[0];
    motion->velocity.y = effect->def->offset_base[1] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[1];
    motion->velocity.z = effect->def->offset_base[2] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[2];
    motion->velocity.w = 0.0f;
    motion->spin.x = effect->def->rotation_base[0] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[0];
    motion->spin.y = effect->def->rotation_base[1] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[1];
    motion->spin.z = effect->def->rotation_base[2] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[2];
    motion->spin.w = 0.0f;
    func_001AF538(effect->def->speed, &motion->accel, &effect->model->rotation);
}
