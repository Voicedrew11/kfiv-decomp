#include "kf_entity.h"

int rand(void);
void func_0018D8B8(Entity *, s32, s32, s32);
int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
void func_0019A310(char *, void (*)(void), int);
void func_001958B8(char *, void (*)(void), int);
void func_00182240(void);

void func_00150818(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_00192C98(entity, 0xE, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        func_0019A310((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (entity->spawn_info->unk40 == 0 && rand() % 100 < entity->params->unk436) {
            ENTITY_FLAG_BITS(entity)->bit57 = 1;
            ENTITY_FLAG_BITS(entity)->bits58 = 0;
            func_001958B8((char *)entity, func_00182240, 1);
        } else if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8((char *)entity, unkE0->unk18, 1);
        }
        break;
    }
}
