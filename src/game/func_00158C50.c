#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00192C98(void *, int, int, u32, int);
float func_001954A8(int, int *);
void func_0018D600(char **, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_00158C50(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x3000;
    switch (entity->state) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk6C = 0;
        entity->unkD6 = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->state++;
        break;
    case 1:
        if ((s16)entity->params->unk434 < unkE0->unk6C++) {
            func_00192C98(entity, 1, 0, 0, 0);
            entity->model->unkD0[1] = func_001954A8((int)entity->model->position, (int *)&D_00413F80);
            func_0018D600((char **)entity, 0x235, 0, (int)entity->model->unkD0);
            entity->state++;
        }
        break;
    case 2:
        entity->unkD6++;
        if (entity->unkD6 > (unkE0->unk7B != 0 ? 0x10 : 0x30)) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkD6 = unkE0->unk7B ? 0x10 : 0x30;
            unkE0->unk68 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
