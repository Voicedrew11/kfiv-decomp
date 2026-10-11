#include "kf_entity.h"

int rand(void);
float sinf(float);
float cosf(float);
int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
float func_00194FB8(float);
void func_001958B8(char *, void (*)(void), int);

void func_00172C78(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            angle = func_00194FB8((f32)(rand() % 361) * 0.017453292f);
            entity->model->position[0] = sinf(angle) * 2000.0f;
            entity->model->position[2] = cosf(angle) * 2000.0f;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
