#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_001639A8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xD, 0, 0xA, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAGS(entity) &= ~0x4000;
            unkE0->unk6A = entity->params->unk43C;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAGS(entity) |= 0x4000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
