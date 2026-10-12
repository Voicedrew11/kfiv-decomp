#include "common.h"
#include "kf_effect.h"


/* Sets the velocity from the heading and speed, and flags the effect. */
void func_001AC400(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;

    velocity = &effect->motion.velocity;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, velocity, &effect->model->rotation);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
