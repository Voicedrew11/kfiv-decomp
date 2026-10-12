#include "kf_effect.h"

void func_001AA330(EffectActor *actor, int frame);

/* Per-frame update: in game modes 3 and 5 the turning state takes the definition's interval. */
void func_001AA3E0(EffectActor *actor, int frame) {
    TurningMotion *motion;
    int mode;
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */

    motion = (TurningMotion *)&actor->motion;
    func_001AA330(actor, frame);
    mode = *(int *)0x70000018;
    if (mode == 3 || mode == 5) {
        motion->unk20 = actor->def->interval;
    }
}
