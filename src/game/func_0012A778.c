#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_001958B8(Entity *, void (*)(Entity *), int);

void func_0012A778(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A != 0) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
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
