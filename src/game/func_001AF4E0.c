#include "kf_effect.h"

int func_001B4F90(void *effect);

/* Returns 1 when the effect has a hit check and func_001B4F90 reports a hit, else 0. */
int func_001AF4E0(EffectActor *effect) {
    if (effect->effect_param != 0xFFFF) {
        if (effect->def->unk10B != 0xFF) {
            if (func_001B4F90(effect) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
