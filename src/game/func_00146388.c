#include "kf_entity.h"

// match: const, so that the store to unk48 can follow the load of it
extern const f32 D_0044680C[];

int func_00192C98(void *, int, int, u32, int);
f32 func_0018B518(f32 *);
void func_0018DCD8(Entity *);

void func_00146388(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 fall = 0.0f;

    entity->unkC6 = 0x40;
    unkE0->unk48 = func_0018B518(entity->model->position);
    if (unkE0->unk48 != D_0044680C[0] && unkE0->unk48 < fall) {
        fall = 2.0f;
    }
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        fall = 0.0f;
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
    entity->unk88 -= fall;
}
