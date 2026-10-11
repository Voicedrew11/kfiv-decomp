#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
s16 func_00190BE0(Entity *);
int func_0019A3F0(void *, int);
void func_001958B8(char *, void (*)(void), int);

void func_00162898(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x4000;
    entity->unkC8 |= func_00190BE0(entity);
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        if (entity->params->unk436 >= entity->model->unkFC->unk82) {
            func_0019A3F0(entity, 0x11);
            entity->unkD6 = ((entity->params->unk436 - entity->model->unkFC->unk82) << 7) / entity->params->unk436;
        } else {
            unkE0->unk68 = entity->params->unk434;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
