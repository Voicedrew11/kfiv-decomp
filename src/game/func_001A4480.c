#include "kf_effect.h"

/* Advances the actor's frame counter and wraps it at the control block's limit. */
void func_001A4480(EffectActor *effect) {
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    effect->frame += 1;
    if (effect->frame >= effect->def->keyframe_end) {
        effect->frame = 0;
    }
}
