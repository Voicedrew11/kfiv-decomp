#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

/* Starts an effect that turns toward a point, lowering the model by 220 units first. */
void func_001AD4B8(EffectActor *effect, EffectSlot *spawn_slot) {
    TurnMotion *motion;

    motion = (TurnMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    effect->model->position.y -= 220.0f;
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->end_frame = func_001B0208((float)effect->def->radius_half, effect, motion, motion->unk10);
    motion->done = 0;
}
