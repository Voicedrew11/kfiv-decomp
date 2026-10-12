#include "common.h"
#include "kf_effect.h"

void func_001AFC08(float *result, float *eased, int value, int min, int max, int mode);
int func_001AFBA8(EffectActor *effect, int *keyframe_frames);
void func_00216778(void *, void *);
void sceVu0ScaleVectorXYZ(void *, void *, float);
void sceVu0AddVector(void *, void *, void *);

/* Sets the model's vector at 0xE0 from the stage vectors; returns the stage, or -1 while blending between two. */
int func_001AEC48(EffectActor *effect) {
    Vector4 scaled_a;
    Vector4 scaled_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->keyframes_a[0];
    keyframe_frames[2] = effect->def->keyframes_a[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = func_001AFBA8(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        func_00216778(&effect->model->unkE0, &effect->def->keys_a[stage]);
        return stage;
    }
    func_001AFC08(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->modes_a[stage]);
    sceVu0ScaleVectorXYZ(&scaled_a, &effect->def->keys_a[stage], blend_a);
    sceVu0ScaleVectorXYZ(&scaled_b, &effect->def->keys_a[stage + 1], blend_b);
    sceVu0AddVector(&effect->model->unkE0, &scaled_a, &scaled_b);
    return -1;
}
