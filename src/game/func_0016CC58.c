#include "kf_entity.h"

// match: the size is unknown; a complete small type would be read through $gp
extern u8 D_0034B21C[];

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_0016CC58(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->unkD6 = 0x20;
        entity->flags |= 0x10000000;
        unkE0->unk7B = 0;
        entity->action_step++;
    case 1:
        if (D_0034B21C[0] != 0) {
            entity->action_step++;
        }
        break;
    case 2:
        entity->unkD6 = unkE0->unk7B * 0x60 / 20 + 0x20;
        unkE0->unk7B++;
        if (unkE0->unk7B > 20) {
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
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
    entity->model->unk15C = entity->unkD6;
}
