#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_001841B0(Entity *entity, int unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u16 timer = unkE0->unk68;

    ENTITY_FLAGS(entity) |= 0x200;
    entity->unkC8 = timer;
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
