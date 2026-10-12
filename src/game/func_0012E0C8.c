#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);

void func_0012E0C8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
