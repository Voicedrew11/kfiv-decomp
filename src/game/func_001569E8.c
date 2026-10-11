#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
float func_00194FB8(float);
s32 func_00157898(Entity *, EntityParamsUnk240 *, s32, s32, s32, f32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_001569E8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
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
        break;
    case 2:
        unkE0->unk1C = 0.0f;
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00157898(entity, unk240, 0, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = func_00157898(entity, unk240, 1, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00157898(entity, unk240, 2, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit55) {
                ENTITY_FLAG_BITS(entity)->bit55 = func_00157898(entity, unk240, 3, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00157898(entity, unk240, 4, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit56) {
                ENTITY_FLAG_BITS(entity)->bit56 = func_00157898(entity, unk240, 5, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
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
