#include "kf_effect.h"

void Vector_Copy(void *, void *);

/* Scales the effect's unk0C by its target's, copies the model pose to and from the target, and wraps the frame at keyframe_end. */
void func_001AE498(EffectActor *effect) {
    EffectMotion *motion;
    Vector4 *motion_rotation;
    int next_frame;

    motion = &effect->motion;
    motion_rotation = &motion->unk10;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    effect->scale *= effect->target->scale * 0.0078125f;
    Vector_Copy(&effect->model->position, &motion->velocity);
    Vector_Copy(&effect->model->rotation, motion_rotation);
    Vector_Copy(&motion->velocity, &effect->target->model->position);
    Vector_Copy(motion_rotation, &effect->target->model->rotation);
    next_frame = effect->frame + 1;
    effect->frame = next_frame;
    if ((u16)next_frame >= effect->def->keyframe_end) { // match: compares the masked 16-bit frame
        effect->frame = 0;
    }
}
