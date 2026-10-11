#include "kf_entity.h"

int rand(void);
void func_001958B8(char *, void (*)(void), int);
void func_001765C0(Entity *);
void func_001766F8(Entity *);

void func_001760B8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;

    if (entity->params->unk10 & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        roll = entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436;
        if (roll != 0) {
            roll = rand() % roll;
        }
        if (roll < entity->params->unk432) {
            func_001958B8((char *)entity, (void (*)(void))func_001765C0, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434) {
            func_001958B8((char *)entity, (void (*)(void))func_001766F8, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436) {
            entity->unkA0 = rand() % 60 + 0x78;
        }
    }
    func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
    return;
}
