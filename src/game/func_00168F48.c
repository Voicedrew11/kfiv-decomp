#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00195AC0(Entity *, void *, s32, s32, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
int func_00199EB8(void *, void *, float);
f32 func_0018A9F0(void *, void *);
float func_00194FB8(float);
float sinf(float);
float cosf(float);
void func_00169500(Entity *);

void func_00168F48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 flag;

    entity->unkC6 |= 0x4000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, 10, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk6E = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 += 0.8f;
        if (unkE0->unk48 > 12.0f) {
            unkE0->unk48 = 12.0f;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
                func_00192C98(entity, 0x12, 0, 1, 1);
                entity->action_step++;
                unkE0->unk48 = 20.0f;
                unkE0->unk44 = 12.0f;
            }
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        if (func_0018A9F0(entity->unk14, entity->model->position) < 220.0f) {
            entity->unk88 = -unkE0->unk48;
            unkE0->unk48 += 0.8f;
            if (unkE0->unk48 > 12.0f) {
                unkE0->unk48 = 12.0f;
            }
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 0x13, 0, 10, 1);
            entity->action_step++;
        }
        if (entity->model->unkFC->unk82 >= 0x10) {
            entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
            entity->unk88 = unkE0->unk48;
            entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
            unkE0->unk44 *= 1.09f;
            unkE0->unk48 -= 0.8f;
            if (unkE0->unk44 > 80.0f) {
                unkE0->unk44 = 80.0f;
            }
            if (unkE0->unk48 < 3.0f) {
                unkE0->unk48 = 3.0f;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            func_001958B8((char *)entity, (void (*)(void))func_00169500, 1);
            return;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            func_001958B8((char *)entity, (void (*)(void))func_00169500, 1);
            return;
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        return;
    }
    return; // match: with this the call above becomes a tail call
}
