#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);

void func_00179C10(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        func_0018DCD8(entity);
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->action_step++;
        }
        break;
    }
}
