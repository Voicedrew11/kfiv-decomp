#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_001958B8(char *, void (*)(void), int);

void func_00152710(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->state) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0xD, 0, 0xA, 1);
        if (unkE0->unk79 == 1) {
            func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0xD, 0, 0xA, 1);
        }
        if (entity->params->unk10 & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->state++;
        break;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAG_BITS(entity)->bit35 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            entity->state++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
