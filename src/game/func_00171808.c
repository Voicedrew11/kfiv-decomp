#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_0019A3F0(void *, int);
void func_0018DCD8(Entity *);

void func_00171808(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->unk434 < entity->model->unkFC->unk82) {
            entity->unkA0 = unkE0->unk68 = func_0019A3F0(entity, entity->unkD1) - entity->model->unkFC->unk82;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / (f32)unkE0->unk68;
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
