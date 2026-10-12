#include "kf_effect.h"

extern EffectActor D_003F3550[];

/* Holds the effect for 6 frames and while any active slot has a type-0xE owner, then ends it. */
void func_001A5CB8(EffectActor *actor) {
    EffectActor *slot;
    int i;

    slot = D_003F3550;
    if (actor->frame < 6) {
        actor->frame++;
        return;
    }
    for (i = 0; i < 0x180; i++, slot++) {
        if (slot->flags & (0x8000L << 41)) {
            if (slot->def->unk100 == 0xE) {
                actor->frame++;
                return;
            }
        }
    }
    func_001A2A18(actor);
}
