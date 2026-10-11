#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00192C98(void *, int, int, u32, int);
float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_00199EB8(void *, void *, float);
void func_001958B8(char *, void (*)(void), int);

void func_00169738(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->state) {
    case 0:
        if (0.0f < func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) * 57.29578f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
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
