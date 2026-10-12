#include "kf_entity.h"

extern u8 D_00349690[0x80][0x40];

int func_00192C98(void *, int, int, u32, int);
float func_001957B8(float, float);
float func_00195388(float *, float, float);
int func_0019A3F0(void *, int);
void func_001958B8(char *, void (*)(void), int);

/* turn on the spot towards Entity.unk68 */
void func_00184330(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 yaw;
    f32 angle;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (entity->spawn_info->unk40 == 1
            && D_00349690[entity->spawn_info->variant][entity->spawn_info->unk42] == entity->spawn_info->unk43) {
            if (0.0f <= angle) {
                func_00192C98(entity, 0x1F, 0, entity->params->unkC4, 0);
            } else {
                func_00192C98(entity, 0x1E, 0, entity->params->unkC4, 0);
            }
        } else {
            if (0.0f <= angle) {
                func_00192C98(entity, 0x1C, 0, entity->params->unkC4, 0);
            } else {
                func_00192C98(entity, 0x1B, 0, entity->params->unkC4, 0);
            }
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = func_0019A3F0(entity, entity->unkD1);
            entity->action_step++;
        }
        break;
    case 2:
        yaw = entity->model->unkD0[1];
        func_00195388(&yaw, entity->unk68, entity->unkA0);
        entity->unk94 += yaw - entity->model->unkD0[1];
        if (ENTITY_FLAG_BITS(entity)->bit34) {
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
