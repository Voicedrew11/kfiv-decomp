#include "kf_entity.h"

int rand(void);
int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
float func_00194FB8(float);
void func_0015AA00(Entity *);

void func_0015A538(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (!(rand() & 1) && ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.5235988f) {
                ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~(3L << 58)) | (1L << 58);
                func_001958B8((char *)entity, (void (*)(void))func_0015AA00, 1);
            } else if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
