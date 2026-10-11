#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_001587A8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->unk7B != 0) {
            entity->unkD6++;
        } else {
            entity->unkD6--;
        }
        if ((unkE0->unk7B == 1 && entity->unkD6 >= 0x30) || (unkE0->unk7B == 0 && entity->unkD6 <= 0x10)) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkD6 = unkE0->unk7B ? 0x30 : 0x10;
            unkE0->unk7B = unkE0->unk7B == 0;
            unkE0->unk6A = entity->params->unk436;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
