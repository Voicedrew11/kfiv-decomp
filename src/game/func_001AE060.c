#include "kf_effect.h"

void Vector_Copy(void *, void *);
int rand(void);

/* Starts an effect that flies off at the definition's speed in a random direction. */
void func_001AE060(EffectActor *effect, EffectSlot *spawn_slot) {
    SpreadMotion *motion = (SpreadMotion *)&effect->motion;
    f32 random_turn;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &effect->motion.velocity, &effect->model->rotation);
    Vector_Copy(&motion->unk10, &effect->model->position);
    random_turn = (rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f;
    motion->unk20 = random_turn * effect->def->unkE4;
    motion->unk24 = effect->def->unkE8;
}
