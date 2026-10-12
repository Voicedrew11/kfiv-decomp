#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);

void func_0014C5B0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        if (entity->spawn_info->unk40 != 0) {
            func_00192C98(entity, 0x12, 0, 0, 1);
        } else {
            func_00192C98(entity, 1, 0, 0, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit17 = 0;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            if (entity->spawn_info->unk40 != 0) {
                func_00192C98(entity, 0x13, 0, entity->params->unkC4, 0);
            } else {
                func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
            }
            if (entity->params->flags & 0x100) {
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
            }
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = 1;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
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
