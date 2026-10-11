#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

void func_001785D8(Entity *entity) {
    if (entity->spawn_info->variant == 1) {
        func_00192C98(entity, 0x12, 0, 0, 0);
    } else {
        func_00192C98(entity, 0x13, 0, 0, 0);
    }
    entity->flags |= 0x40000;
    entity->flags |= 0x100000;
    entity->flags |= 0x200000;
    entity->flags |= 0x400000;
    entity->flags |= 0x800000;
    entity->flags |= 0x1000000;
    entity->flags |= 0x2000000;
    entity->flags |= 0x4000000;
    entity->flags |= 0x10000;
    entity->flags |= 0x10000000;
}
