#include "kf_entity.h"

extern float D_00413F80[];

int func_00192C98(void *, int, int, u32, int);
float func_001957B8(float, float);
float func_00195478(float *, char *);
void func_001958B8(char *, void (*)(void), int);

void func_001625A0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->state) {
    case 0:
        if (func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position)) > 0.0f) {
            func_00192C98(entity, 0x1D, 0, 5, 1);
        } else {
            func_00192C98(entity, 0x1E, 0, 5, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
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
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
