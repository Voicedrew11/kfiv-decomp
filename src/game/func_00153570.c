#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_0019A290(Entity *, s32);
// match: returns int; the ignored result is what keeps $2 out of the flag test below
int func_0019A310(char *, void (*)(void), int);
void func_001958B8(char *, void (*)(void), int);

void func_00153570(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        if (unkE0->unk79 == 0) {
            func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0xF, 0, 5, 1);
        }
        if (entity->params->unk10 & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_0019A290(entity, entity->params->unkC8);
            func_0019A310((char *)entity, NULL, 0);
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
