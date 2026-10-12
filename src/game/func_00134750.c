#include "kf_entity.h"

extern u32 D_00446ED4;

int func_0019A448(void *, void *, int, int, int, int, int);
int func_00199EB8(void *, void *, float);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_00134750(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= unk3D8->unk01) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0;
        }
        if (((EntityUnkE0SlotBits *)&unkE0->unk80)->index == 0x17) {
            if (unkE0->unk80.unk02 >= 20 && unkE0->unk80.unk02 <= 44) {
                if (D_00446ED4 & 1) {
                    func_00195ED0(entity, unk3D8, 20, 44);
                }
                ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            }
        } else {
            func_00195ED0(entity, unk3D8,
                          (unkE0->unk80.unk04 - entity->params->unk80[((EntityUnkE0SlotBits *)&unkE0->unk80)->index]) >> 8,
                          unkE0->unk80.unk02);
        }
        if (unkE0->unk80.unk02 > 0 && unk3D8->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
