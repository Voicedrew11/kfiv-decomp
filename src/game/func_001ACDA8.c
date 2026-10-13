#include "common.h"
#include "kf_effect.h"


/* Ends the effect when its flag byte is set; otherwise runs one frame of its channels. */
void func_001ACDA8(EffectActor *effect) {
    if (*(s8 *)&effect->motion != 0) { // match: velocity.x's first byte is read as a signed byte
        return KF_Effect_Free(effect);
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    effect->frame += 1;
}
