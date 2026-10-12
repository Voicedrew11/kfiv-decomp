#include "kf_effect.h"

/* Starts an effect with the definition's initial velocity.x and velocity.y. */
void func_001AAA60(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;
    EffectDef *def;

    velocity = &effect->motion.velocity;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    def = effect->def;
    velocity->x = def->unkDC;
    velocity->y = def->unkE0;
}
