#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_00181AE0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
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
