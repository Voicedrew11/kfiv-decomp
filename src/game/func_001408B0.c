#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
void func_001409D0(Entity *);

void func_001408B0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->distance_to_player < entity->params->unk436 || unkE0->unk7A != 0) {
        ENTITY_FLAG_BITS(entity)->bit32 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit33 = TRUE;
        unkE0->unk7A = 1;
        func_001409D0(entity);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 0);
        } else {
            func_001958B8((char *)entity, unkE0->unk18, 0);
        }
    }
}
