#include "kf_entity.h"

int rand(void);
int func_00192C98(void *, int, int, u32, int);
float func_00194FB8(float);
float func_00195388(float *, float, float);
void func_00195588(f32 *, f32);
void func_0018D600(char **, int, u32, int);
s32 func_0018B150(f32 *, f32 *, f32, f32 *);
void func_00216778(void *, void *);
float func_00194F80(void *, void *);

void func_001368B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 bounds[4];
    f32 direction[4];
    f32 heading;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 4, 0, entity->params->unkC4, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
        }
        unkE0->unk48 = entity->unk68 + 1.5707964f + (f32)(rand() % 180) * 0.017453292f;
        unkE0->unk48 = func_00194FB8(unkE0->unk48);
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        entity->action_step++;
        break;
    case 1:
        heading = entity->model->unkD0[1];
        func_00195388(&heading, unkE0->unk48, 10.0f);
        entity->unk94 += heading - entity->model->unkD0[1];
        bounds[0] = entity->model->position[0] - entity->radius;
        bounds[1] = entity->model->position[0] + entity->radius;
        bounds[2] = entity->model->position[2] - entity->radius;
        bounds[3] = entity->model->position[2] + entity->radius;
        func_00195588(direction, entity->model->unkD0[1]);
        if ((s32)(entity->flags >> 52) & 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
            entity->unkA0 = 20;
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step = 3;
        } else if (func_0018B150(entity->model->position, direction, entity->radius, bounds)) {
            func_00216778(unkE0->unk30, entity->model->position);
            ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        if (func_00194F80(entity->model->position, unkE0->unk30) > entity->radius * 4.0f) {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
