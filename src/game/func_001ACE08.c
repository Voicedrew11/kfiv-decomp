#include "common.h"
#include "kf_effect.h"

void Vector_Copy(void *, void *);

/* Spawns the effect: keeps its argument, then starts it from its two stored positions. */
void func_001ACE08(EffectActor *effect, EffectSlot *spawn_slot) {
    SavedArgMotion *motion;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion = (SavedArgMotion *)&effect->motion;
    motion->owner_entity = effect->owner_entity;
    Vector_Copy(&effect->model->position, effect->start_position);
    Vector_Copy(&effect->model->rotation, effect->start_rotation);
    motion->unk14 = 0;
}
