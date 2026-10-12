#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_00180DE8(Entity *entity, s32 unused) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    switch (entity->action_step) {
    case 0:
        ((EntityUnkE0Kind150588 *)unkE0)->unk1C = 0;
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 1:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
