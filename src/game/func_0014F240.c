#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_0018D600(char **, int, u32, int);
void func_00150478(Entity *entity);
void func_001958B8(char *, void (*)(void), int);

void func_0014F240(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, entity->unkE0.unk88, 1, 0x11, 0, entity->params->unkC4, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk02 == entity->params->unk438) {
            func_0018D600((char **)entity, entity->params->unk436, entity->params->unk43E, (int)entity->model->unkD0);
        }
        if (entity->params->unk43C < ((EntityUnkE0Slot *)unkE0->unk88)->unk02) {
            func_0018D600((char **)entity, entity->params->unk43A, entity->params->unk43E, (int)entity->model->unkD0);
            func_00150478(entity);
            entity->action_step++;
        }
        break;
    case 3:
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
