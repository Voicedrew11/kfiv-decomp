#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern char D_00413F80[];
extern char D_0043EF20[];
extern char D_0043EF38[];
extern int D_004468CC;
extern int D_004468D0;
int printf(const char *, ...);
void func_001A19F0(void) {
    EffectSlot *slot = D_003F1750;
    EffectActor *actor = D_003F3550;
    int i;

    for (i = 0; i < 0x60; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            printf(D_0043EF20, i, slot->owner_entity);
            if (!(slot->flags & EFFECT_SLOT_LIVE)) {
                continue;
            }
            if (slot->owner_entity == (Entity *)D_00413F80) {
                continue;
            }
            func_001A2AF8(slot);
            if (D_004468CC <= 0) {
                break;
            }
        }
    }

    for (i = 0; i < 0x180; i++, actor = (EffectActor *)((char *)actor + 0xA0)) {
        if (actor->flags & EFFECT_FLAG_LIVE) {
            printf(D_0043EF38, i, actor->owner_entity);
            if (!(actor->flags & EFFECT_FLAG_LIVE)) {
                continue;
            }
            if (actor->owner_entity == (Entity *)D_00413F80) {
                continue;
            }
            func_001A2A18(actor);
            if (D_004468D0 <= 0) {
                break;
            }
        }
    }
}
