#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
void func_00150478(Entity *entity);
void func_001958B8(char *, void (*)(void), int);

void func_00150200(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 1);
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        if (unkE0->unk8A == entity->params->unk438) {
            func_0018D600((char **)entity, entity->params->unk436, entity->params->unk43E, (int)entity->model->unkD0);
        }
        if (entity->params->unk43C < entity->model->unkFC->unk82) {
            func_0018D600((char **)entity, entity->params->unk43A, entity->params->unk43E, (int)entity->model->unkD0);
            func_00150478(entity);
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
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
