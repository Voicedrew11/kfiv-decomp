#include "kf_effect.h"

/* Start-up helper: stores how far the spawner's level is below the definition's max_level. */
void func_001A99D8(EffectActor *actor, EffectSlot *spawn_slot) {
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    *(int *)&actor->motion = actor->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level;
}
