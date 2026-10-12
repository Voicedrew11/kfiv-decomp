#include "kf_effect.h"

void func_00216778(void *, void *);

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
    *(f32 *)&effect->unk0C = *(f32 *)&effect->unk0C * (*(f32 *)&effect->target->unk0C * 0.0078125f); // match: unk0C is a float here
    func_00216778(&effect->model->position, &motion->velocity);
    func_00216778(&effect->model->rotation, motion_rotation);
    func_00216778(&motion->velocity, &effect->target->model->position);
    func_00216778(motion_rotation, &effect->target->model->rotation);
    next_frame = effect->frame + 1;
    effect->frame = next_frame;
    if ((u16)next_frame >= effect->def->keyframe_end) { // match: compares the masked 16-bit frame
        effect->frame = 0;
    }
}
