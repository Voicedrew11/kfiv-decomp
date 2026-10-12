#include "common.h"
#include "kf_effect.h"

void func_001AFC08(float *result, float *eased, int value, int min, int max, int mode);
int func_001AFBA8(EffectActor *effect, int *keyframe_frames);
void sceVu0ScaleVectorXYZ(void *, void *, float);
void sceVu0AddVector(void *, void *, void *);

/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* Adds the stage rotation to the model's rotation, wrapping each angle; returns the stage, or -1 while blending between two. */
int func_001AED58(EffectActor *effect) {
    Vector4 scaled_a;
    Vector4 scaled_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->keyframes_b[0];
    keyframe_frames[2] = effect->def->keyframes_b[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = func_001AFBA8(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        sceVu0AddVector(&effect->model->rotation, &effect->model->rotation, &effect->def->keys_b[stage]);
        effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
        effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
        effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
        return stage;
    }
    func_001AFC08(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->modes_b[stage]);
    sceVu0ScaleVectorXYZ(&scaled_a, &effect->def->keys_b[stage], blend_a);
    sceVu0ScaleVectorXYZ(&scaled_b, &effect->def->keys_b[stage + 1], blend_b);
    sceVu0AddVector(&scaled_a, &scaled_a, &scaled_b);
    sceVu0AddVector(&effect->model->rotation, &scaled_a, &effect->model->rotation);
    effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
    effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
    effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
    return -1;
}
