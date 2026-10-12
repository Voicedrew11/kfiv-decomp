#include "common.h"
#include "kf_effect.h"

void func_001AFC08(float *result, float *eased, int value, int min, int max, int mode);
int func_001AFBA8(EffectActor *effect, int *keyframe_frames);

/* Sets the effect's colour from the stage colours; returns the stage, or -1 while blending between two. */
int func_001AF0B0(EffectActor *effect) {
    Vector4 unused_a; // match: retail reserves the two vectors its sibling functions scale into
    Vector4 unused_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->colour_keyframes[0];
    keyframe_frames[2] = effect->def->colour_keyframes[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = func_001AFBA8(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        EFFECT_COLOUR(effect)->red = effect->def->colour_keys[stage][0];
        EFFECT_COLOUR(effect)->green = effect->def->colour_keys[stage][1];
        EFFECT_COLOUR(effect)->blue = effect->def->colour_keys[stage][2];
        EFFECT_COLOUR(effect)->alpha = effect->def->colour_keys[stage][3];
        return stage;
    }
    func_001AFC08(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->colour_modes[stage]);
    EFFECT_COLOUR(effect)->red = effect->def->colour_keys[stage][0] * blend_a + effect->def->colour_keys[stage + 1][0] * blend_b;
    EFFECT_COLOUR(effect)->green = effect->def->colour_keys[stage][1] * blend_a + effect->def->colour_keys[stage + 1][1] * blend_b;
    EFFECT_COLOUR(effect)->blue = effect->def->colour_keys[stage][2] * blend_a + effect->def->colour_keys[stage + 1][2] * blend_b;
    EFFECT_COLOUR(effect)->alpha = effect->def->colour_keys[stage][3] * blend_a + effect->def->colour_keys[stage + 1][3] * blend_b;
    return -1;
}
