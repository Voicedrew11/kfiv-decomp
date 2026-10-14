#include "kf_effect.h"

void func_001AF298(void *, void *);
void Vector_Copy(void *, void *);

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
    Vector_Copy(&effect->model->position, velocity);
    Vector_Copy(&effect->model->rotation, motion_unk10);
    Vector_Copy(velocity, &effect->target->model->position);
    Vector_Copy(motion_unk10, &effect->target->model->rotation);
    func_001AF298(effect, &state);
    effect->frame++;
}
