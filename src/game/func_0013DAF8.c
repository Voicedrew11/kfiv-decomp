#include "kf_entity.h"
#include "kf_effect.h"

float sqrtf(float);
float atan2f(float, float);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
s32 func_00195B68(Entity *, void *, s32, s32, s32, f32);
int rand(void);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_0013D9D0(Entity *);
void func_0013E1B8(Entity *);

void func_0013DAF8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    f32 position[4];
    f32 angles[4];
    f32 *tip;
    f32 *base;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        base = (f32 *)func_0010CAB8((char *)entity->model, 8);
        if (tip != NULL && base != NULL) {
            if (entity->model->unkFC->unk82 >= 10 && entity->model->unkFC->unk82 <= 22) {
                delta_x = base[0] - tip[0];
                delta_y = base[1] - tip[1];
                delta_z = base[2] - tip[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = tip[0] - unkE0->unk4C;
                delta_y = tip[1] - unkE0->unk50;
                delta_z = tip[2] - unkE0->unk54;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk4C + delta_x * i / count;
                    position[1] = unkE0->unk50 + delta_y * i / count;
                    position[2] = unkE0->unk54 + delta_z * i / count;
                    Effect_StartSpawner(0x222, position, angles, entity);
                }
            }
            unkE0->unk4C = tip[0];
            unkE0->unk50 = tip[1];
            unkE0->unk54 = tip[2];
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195B68(entity, unk240, 0, frame, entity->model->unkFC->unk82, func_00194FB8(entity->unk68));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195B68(entity, unk240, 1, frame, entity->model->unkFC->unk82, func_00194FB8(entity->unk68));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195B68(entity, unk240, 2, frame, entity->model->unkFC->unk82, func_00194FB8(entity->unk68));
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (rand() % 100 < entity->params->unk432) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                unkE0->unk68 = entity->params->unk430 / 2 + rand() % entity->params->unk430;
                ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
                func_00192C98(entity, 1, 0, 5, 1);
                func_001958B8((char *)entity, (void (*)(void))func_0013D9D0, 1);
            } else {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                func_0019A290(entity, unk240->unk02);
                ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
                func_001958B8((char *)entity, (void (*)(void))func_0013E1B8, 1);
            }
        }
        break;
    }
}
