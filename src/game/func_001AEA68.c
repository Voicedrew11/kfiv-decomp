#include "common.h"
#include "kf_effect.h"

/* Clears the effect's velocity word and its target. */
void func_001AEA68(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    effect->target = NULL;
    *(int *)&effect->motion = 0; // match: an int store of the velocity's first word (sw $0)
}
