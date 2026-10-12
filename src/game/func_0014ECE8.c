#include "kf_entity.h"

extern float D_00413F80[];

int func_0019A448(void *, void *, int, int, int, int, int);
void func_00216778(void *, void *);
int func_00195E48(void *, int, void *, void *);
void func_0018DCD8(Entity *);

void func_0014ECE8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 target[4];

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &slots->unk80, 0, 2, 0, entity->params->unkC4, 0);
        func_0019A448(entity, &slots->unk88, 1, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_0019A448(entity, &slots->unk80, 0, 0x10, 5, entity->params->unkC4, 1);
            func_0019A448(entity, &slots->unk88, 1, 0x10, 5, entity->params->unkC4, 1);
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->bits2 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 1:
    case 3:
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if ((slots->unk88.unk04 - entity->params->unk80[slots->unk88.index]) >> 8 < 0x2B && slots->unk88.unk02 >= 0x2B) {
            func_00216778(target, D_00413F80);
            func_00195E48(entity, ((EntityParamsUnk3D8Full *)&entity->params->unk3D8[0])->unk10[0].unk00, target,
                          entity->model->unkD0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 5:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
