#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"

void Sound_PlaySw(int mode);
extern u32 D_00402D50[14];
extern PlayerState D_00413F80;

void func_001AEB30(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectDef *def;
    u32 *key;
    u32 *param;
    int i;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    def = effect->def;
    if (def->radius_half == 2) {
        Sound_PlaySw(def->radius_half);
    } else if (def->radius_half == 3) {
        // match: the table walk is two pointers stepped together; this loop form keeps retail's registers
        for (i = 0, param = &D_00402D50[1], key = &D_00402D50[0]; i < 7; key += 2, i++) {
            if (D_00413F80.weapon == *key && effect->effect_param == *param) {
                Sound_PlaySw(def->radius_half);
                break;
            }
            param += 2;
        }
    }
}
