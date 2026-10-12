#include "kf_effect.h"

void func_001AF298(void *, void *);
void func_00216778(void *, void *);

/* Copies the model's and the target's position and rotation into the motion, then counts a frame. */
void func_001AA770(EffectActor *effect) {
    Vector4 *velocity;
    Vector4 *motion_unk10;
    u8 state;

    state = 0;
    velocity = &effect->motion.velocity;
    motion_unk10 = &effect->motion.unk10;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    func_00216778(&effect->model->position, velocity);
    func_00216778(&effect->model->rotation, motion_unk10);
    func_00216778(velocity, &effect->target->model->position);
    func_00216778(motion_unk10, &effect->target->model->rotation);
    func_001AF298(effect, &state);
    effect->frame++;
}
