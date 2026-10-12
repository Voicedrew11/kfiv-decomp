#include "kf_effect.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);
void func_001AF538(float *, float *, float);

/* Re-seeds the motion from the model and definition. */
void func_001AE910(EffectActor *actor, int frame) {
    EffectMotion *motion;

    motion = &actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    func_001AF538(&motion->velocity.x, &actor->model->rotation.x, actor->def->speed);
}
