#include "kf_effect.h"

void func_00216778(void *, void *);

/* Starts an effect at rest, with its model's position and a fixed 0.1 at +0x20 and +0x24. */
void func_001AE730(EffectActor *effect, EffectSlot *spawn_slot) {
    StartMotion *motion = (StartMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_00216778(&motion->unk10, &effect->model->position);
    motion->velocity.z = effect->def->unkE0;
    effect->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->unk24 = motion->unk20 = 0.1f;
}
