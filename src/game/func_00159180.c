#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

float atanf(float);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
void func_00216778(void *, void *);
float func_00194FB8(float);
int func_0018D4E8(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_00159180(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= 0x1C && entity->model->unkFC->unk82 <= 0x3C) {
            func_00216778(direction, entity->model->unkD0);
            direction[0] = atanf((entity->model->position[1] - D_00413F80.position[1]) / entity->distance_to_player);
            direction[1] = func_00194FB8(direction[1] - 3.1415927f);
            if (D_00446ED4 & 1) {
                func_0018D4E8(entity, unk3D8, 0x1C, 0x3C, direction);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
