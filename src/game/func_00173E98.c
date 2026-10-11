#include "kf_entity.h"

void func_0018D8B8(Entity *, s32, s32, s32);
int func_0019A448(void *, void *, int, int, int, int, int);
void func_0019A290(Entity *, s32);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_001958B8(char *, void (*)(void), int);

void func_00173E98(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_0019A448(entity, &unkE0->unk80, 0, 0xE, 0, 5, 1);
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, 5, 0);
        if (entity->params->unk10 & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        func_0019A310((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
        }
        break;
    }
}
