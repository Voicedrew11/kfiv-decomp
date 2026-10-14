#include "kf_effect.h"

void Vector_Copy(void *, void *);

/* Follower effect: copies its target's position and rotation into its own motion. */
void func_001AA728(EffectActor *effect) {
    Vector_Copy(&effect->motion, &effect->target->model->position);
    Vector_Copy(&effect->motion.unk10, &effect->target->model->rotation);
}
