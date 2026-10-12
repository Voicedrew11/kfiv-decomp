#include "kf_effect.h"

extern u8 D_0034AFD6[];

/* Advances the frame counter and wraps it at radius_half until the loop is stopped. */
void func_001AEAB0(EffectActor *effect) {
    LoopMotion *motion;

    func_001AEC48(effect);
    motion = (LoopMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    effect->frame += 1;
    if (D_0034AFD6[0] == 3) {
        motion->stopped = 1;
    }
    if (motion->stopped == 0) {
        if (effect->frame >= effect->def->radius_half) {
            effect->frame = 0;
        }
    }
}
