#include "common.h"
#include "kf_linked_point.h"
#include "kf_effect.h"

extern int D_004468C8;
extern int D_004468D0;

void func_0019CD70(s8 handle);

/* Frees an effect, then the spawner slots and effects that it owns or targets. */
void func_001A2A18(void *effect) {
    EffectActor *actor = (EffectActor *)effect;

    EFFECT_FLAG_BITS(actor)->live = 0;
    actor->unk90 = 4;
    D_004468D0--;
    if (EFFECT_FLAG_PARTS(actor)->unk46 > 0) {
        D_004468C8--;
    }
    func_0019CD70(EFFECT_FLAG_PARTS(actor)->unk46);
    EFFECT_FLAG_PARTS(actor)->unk46 = -1;
    if (EFFECT_FLAG_PARTS(actor)->unk44 != -1) {
        KF_LinkedPoint_ReleaseIndex(EFFECT_FLAG_PARTS(actor)->unk44);
    }
    if (actor->flags & EFFECT_FLAG_HAS_SLOTS) {
        func_001A2BC8(actor);
    }
    if (actor->flags & EFFECT_FLAG_HAS_TARGETERS) {
        func_001A2B28(actor);
    }
}
