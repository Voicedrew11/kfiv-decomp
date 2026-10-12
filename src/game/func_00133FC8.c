#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_0019A290(Entity *, s32);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_001958B8(char *, void (*)(void), int);

void func_00133FC8(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
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
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
