#include "common.h"
#include "kf_effect.h"

void func_001AF298(EffectActor *effect, s8 *state);

// Counts the countdown down, and calls func_001AF298 once it runs out while the flag is clear.
void func_001A4380(EffectActor *effect) {
    CountdownMotion *motion = (CountdownMotion *)&effect->motion;
    s8 state_buffer[0x10];
    int remaining;

    remaining = motion->countdown - 1;
    motion->countdown = remaining;
    if (motion->unk04 == 0 && remaining <= 0) {
        func_001AF298(effect, state_buffer);
    }
    effect->frame += 1;
}
