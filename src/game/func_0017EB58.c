#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_0018D600(char **, int, u32, int);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);
void func_0017E5D0(Entity *);

void func_0017EB58(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &slots->unk80, 0, 1, 0, 0, 1);
        func_0019A448(entity, &slots->unk88, 1, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            unkE0->unk50 = entity->params->unk43A;
            func_0018D600((char **)entity, 0x166, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 3.0f;
        if (unkE0->unk50 <= 0.0f) {
            func_0019A448(entity, &slots->unk80, 0, 2, 0, entity->params->unkC4, 1);
            func_0019A448(entity, &slots->unk88, 1, 2, 0, entity->params->unkC4, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 1.0f;
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (slots->unk80.unk02 < 0x32) {
            entity->model->position[1] -= unkE0->unk50;
            unkE0->unk50 -= 0.1f;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        entity->unkC8 = 1;
        func_0017E5D0(entity);
        break;
    }
}
