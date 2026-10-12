#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
void func_001958B8(char *, void (*)(void), int);

void func_00132428(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7B != 0) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
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
