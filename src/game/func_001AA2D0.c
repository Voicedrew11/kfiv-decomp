#include "kf_effect.h"

void func_001AF298(char *, char *);

/* Pulse effect step: while the frame is in [radius_half, interval) it runs func_001AF298 on the actor. */
void func_001AA2D0(EffectActor *effect) {
    PulseMotion *motion;
    u8 hit;

    hit = 0;
    motion = (PulseMotion *)&effect->motion;
    if (effect->frame >= motion->radius_half && effect->frame < motion->interval) {
        func_001AF298((char *)effect, (char *)&hit);
    }
    effect->frame += 1;
}
