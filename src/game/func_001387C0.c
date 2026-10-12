#include "kf_entity.h"

float func_001957B8(float, float);
int func_00192C98(void *, int, int, u32, int);
int func_0019A3F0(void *, int);
float func_00195388(float *, float, float);

void func_001387C0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 frames;

    entity->unkC6 = 0x1000;
    switch (entity->action_step) {
    case 0:
        if (func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14) >= 0.0f) {
            func_00192C98(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            func_00192C98(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            frames = func_0019A3F0(entity, entity->unkD1);
            entity->unkA0 = frames;
            if (entity->unk28 != 0) {
                entity->unkA0 = frames * 2;
            }
            entity->action_step++;
        }
        break;
    case 2:
        func_00195388(&entity->model->unkD0[1], entity->spawn_info->unk14, entity->unkA0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk48 = entity->model->position[0];
            unkE0->unk4C = entity->model->position[1];
            unkE0->unk50 = entity->model->position[2];
            entity->prev_state = entity->state;
            entity->state = 0;
            unkE0->unk00(entity, 1);
        }
        break;
    }
}
