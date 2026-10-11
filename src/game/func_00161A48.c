#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
s16 func_00190BE0(Entity *);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_00161A48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 2, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
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
            entity->unkC8 |= func_00190BE0(entity);
        }
        return;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, frame, entity->model->unkFC->unk82);
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
