#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00195E48(void *, int, void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_0016C888(Entity *);

void func_0016DCC0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0x12, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x10) {
            func_00195E48(entity, 0xC9, entity->model->position, entity->model->unkD0);
            unkE0->unk6A = 0x5A;
            entity->state++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
                return;
            }
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            return;
        }
        break;
    }
    func_0016C888(entity);
}
