#include "kf_effect.h"

void func_00216778(void *, void *);

/* Follower effect: copies its target's position and rotation into its own motion. */
void func_001AA728(EffectActor *effect) {
    func_00216778(&effect->motion, &effect->target->model->position);
    func_00216778(&effect->motion.unk10, &effect->target->model->rotation);
}
