#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

/* Sends the entity back to its spawn point with the walk handler of its kind. */
void func_00126C28(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    f32 unused[8]; // match: retail reserves 0x20 bytes, which also keeps the call from being a jump

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->target = entity->spawn_info;
    entity->unkC8 = 1;
    entity->unkA0 = 0;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
