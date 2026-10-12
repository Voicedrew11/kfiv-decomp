#include "kf_entity.h"

s32 func_0018CAF8(Entity *);
s32 func_0018C728(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_0013DAF8(Entity *);
void func_0013E1B8(Entity *);

void func_0013D9D0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    if (entity->distance_to_player < (s16)entity->params->unk434 + entity->radius + 100.0f) {
        if (!func_0018CAF8(entity) && !func_0018C728(entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_0013DAF8, 1);
        }
    }
    if (unkE0->unk68-- <= 0 && entity->action_step == 0) {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        func_001958B8((char *)entity, (void (*)(void))func_0013E1B8, 1);
    }
}
