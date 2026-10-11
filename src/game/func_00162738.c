#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
s16 func_00190BE0(Entity *);
void func_001958B8(char *, void (*)(void), int);

void func_00162738(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    entity->flags |= 0x200;
    entity->unkC8 |= func_00190BE0(entity);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x12, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
