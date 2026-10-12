#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_0013B9E8(Entity *entity, s32 unknown) {
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
