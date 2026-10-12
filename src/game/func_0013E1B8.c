#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);

void func_0013E1B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
