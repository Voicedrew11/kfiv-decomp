#include "common.h"
#include "kf_effect.h"

void func_00216778(void *, void *);

/* Spawns the effect: keeps its argument, then starts it from its two stored positions. */
void func_001ACE08(EffectActor *effect, EffectSlot *spawn_slot) {
    SavedArgMotion *motion;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion = (SavedArgMotion *)&effect->motion;
    motion->owner_entity = effect->owner_entity;
    func_00216778(&effect->model->position, effect->start_position);
    func_00216778(&effect->model->rotation, effect->start_rotation);
    motion->unk14 = 0;
}
