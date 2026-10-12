#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
float func_00195388(float *, float, float);
void func_0018DCD8(Entity *);

/* Action: dies. Levels out, plays the death animation, drops what it carries and fades away. */
void func_00129780(Entity *entity) {
    entity->unkC6 = 0x40;
    func_00195388(&entity->model->unkD0[0], 0.0f, 6.0f);
    func_00195388(&entity->model->unkD0[2], 0.0f, 6.0f);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
