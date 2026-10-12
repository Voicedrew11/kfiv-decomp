#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"
#include "kf_screen.h"

extern EffectActor D_003F3550[0x180];
extern ModelInstance D_0054A260[0x180];
extern void (*D_00402550[][2])();
extern FxTableEntry D_00402DA8[];
extern PlayerState D_00413F80;
extern int D_004468C8;
extern int D_004468D0;

void func_0020B2E8(void *, u32);
void func_001A2C68(int);
char func_0019C8D0(int index, int first, int second, int third, int fourth);
AreaNode *func_00115950(f32 *point, f32 margin);

/* Runs a spawner slot for one frame: after its start delay, every unkA7 frames, it makes unkA5
   effects of its definition and counts its level down, freeing itself at 0. */
void func_001A24D8(EffectSlot *slot) {
    EffectDef *def;
    EffectActor *effect;
    int index;
    int made;
    s64 placement;

    if (EFFECT_SLOT_VIEW(slot)->unk27 != 0) {
        EFFECT_SLOT_VIEW(slot)->unk27--;
        return;
    }
    def = EFFECT_SLOT_VIEW(slot)->def;
    if (def->unkA7 != 0) {
        u8 timer = EFFECT_SLOT_VIEW(slot)->unk26--;

        if (timer != 0) {
            return;
        }
        EFFECT_SLOT_VIEW(slot)->unk26 = def->unkA7;
    }
    if (D_004468D0 >= 0x180) {
        if (--EFFECT_SLOT_VIEW(slot)->level == 0) {
            func_001A2AF8(slot);
        }
        return;
    }
    for (made = 0; made < def->unkA5; made++) {
        effect = D_003F3550;
        for (index = 0; index < 0x180; index++, effect++) {
            if (!(effect->flags & EFFECT_FLAG_LIVE) && effect->unk90 == 0) {
                break;
            }
        }
        if (index >= 0x180) {
            break;
        }
        D_004468D0++;
        func_0020B2E8(effect, 0x28);
        EFFECT_FLAG_BITS(effect)->live = TRUE;
        effect->frame = 0;
        effect->start_position = slot->unk30;
        effect->model = &D_0054A260[index];
        effect->def = EFFECT_SLOT_VIEW(slot)->def;
        effect->owner_entity = slot->owner_entity;
        effect->target = slot->owner_effect;
        effect->effect_param = slot->effect_param;
        effect->owner = slot->owner;
        effect->start_rotation = EFFECT_SLOT_VIEW(slot)->start_rotation;
        EFFECT_FLAG_PARTS(effect)->unk44 = -1;
        effect->unk90 = 4; /* frames the actor stays reserved once it is freed */
        effect->unk6C = 0;
        func_001A2C68(index);
        D_00402550[effect->def->unk100][0](effect, slot);
        EFFECT_FLAG_BITS(effect)->bit59 = effect->def->unk11A != 0;
        if (D_00402DA8[effect->effect_param].unk102 != 0) {
            EFFECT_FLAG_PARTS(effect)->unk46 = func_0019C8D0(EFFECT_FLAG_BITS(effect)->bit59, effect->def->unk11A, 0, (int)&D_00413F80, EFFECT_FLAG_PARTS(effect)->unk46);
        } else if (EFFECT_FLAG_BITS(effect)->bit59 || effect->effect_param == D_00413F80.unk3D) {
            EFFECT_FLAG_PARTS(effect)->unk46 = func_0019C8D0(EFFECT_FLAG_BITS(effect)->bit59, effect->def->unk11A, 0, (int)&effect->model->position, EFFECT_FLAG_PARTS(effect)->unk46);
        }
        if (EFFECT_FLAG_PARTS(effect)->unk46 > 0) {
            D_004468C8++;
        }
        placement = EFFECT_DEF_FLAGS(effect->def) & 0x600000;
        if (placement == 0 || placement == 0x600000) {
            effect->unk58 = (char *)func_00115950((f32 *)&effect->model->position, 0.0f);
        }
        def = EFFECT_SLOT_VIEW(slot)->def;
    }
    if (--EFFECT_SLOT_VIEW(slot)->level == 0) {
        func_001A2AF8(slot);
    }
}
