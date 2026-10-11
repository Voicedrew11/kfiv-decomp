#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);
void *func_001267B0(int);

void func_001679B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) &= ~0x200;
        if (unkE0->unk7C != 0) {
            Entity *other = func_001267B0(unkE0->unk7B);

            if (other != NULL) {
                other->unkE0.unk7A = -1;
            }
        }
        unkE0->unk7C = 0;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            ENTITY_FLAGS(entity) |= 0x8000000;
            ENTITY_FLAGS(entity) |= 0x4000000;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->state++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}
