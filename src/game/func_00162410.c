#include "kf_entity.h"

int rand(void);
int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
void func_00162738(Entity *);

void func_00162410(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0xA, 0, 5, 1);
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
            if (rand() % 100 < 50) {
                func_001958B8((char *)entity, (void (*)(void))func_00162738, 1);
            } else if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
