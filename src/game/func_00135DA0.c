#include "kf_entity.h"

int func_0019A3F0(void *, int);
int func_00192C98(void *, int, int, u32, int);
void func_00216778(void *, void *);
void func_001958B8(char *, void (*)(void), int);

void func_00135DA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 2, func_0019A3F0(entity, 2) - 1, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit15 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
            func_00216778(entity->model->unkD0, &entity->spawn_info->unk10);
            func_001958B8((char *)entity, unkE0->unk10, 1);
        }
        break;
    }
}
