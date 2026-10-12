#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
void func_0019A310(char *, void (*)(void), int);
void func_001958B8(char *, void (*)(void), int);
void func_0013D9D0(Entity *);

void func_0013D820(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        }
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
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
            func_00192C98(entity, 1, 0, 5, 1);
            func_001958B8((char *)entity, (void (*)(void))func_0013D9D0, 1);
        }
        break;
    }
}
