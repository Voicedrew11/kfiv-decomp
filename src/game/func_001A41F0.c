#include "common.h"
#include "kf_effect.h"

void func_001AF298(EffectActor *effect, s8 *state);

// Counts the countdown down, and calls func_001AF298 on the frame that matches def->radius_half.
void func_001A41F0(EffectActor *effect) {
    CountdownMotion *motion = (CountdownMotion *)&effect->motion;
    s8 state;

    motion->countdown -= 1;
    if (effect->def->radius_half == effect->frame) {
        func_001AF298(effect, &state);
    }
    effect->frame += 1;
}
