#include "kf_entity.h"

extern float D_00413F80[];
extern u8 D_0038CC57[];

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
float func_00195478(float *, char *);
float sinf(float);
float cosf(float);
float func_00194FB8(float);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_00145080(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = (EntityParamsUnk240 *)entity->params->unk350;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x16, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        unkE0->unk7A = 0;
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
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit41 == 1) {
            unkE0->unk7A = 1;
        }
        if (entity->model->unkFC->unk82 >= 0x1B && entity->model->unkFC->unk82 <= 0x1F && unkE0->unk7A == 1) {
            D_0038CC57[0] = 0;
            if (entity->radius + 50.0f + 100.0f < entity->distance_to_player) {
                D_00413F80[0] -= sinf(func_00194FB8(func_00195478(entity->model->position, (char *)D_00413F80))) * 30.0f;
                D_00413F80[2] -= cosf(func_00194FB8(func_00195478(entity->model->position, (char *)D_00413F80))) * 30.0f;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
