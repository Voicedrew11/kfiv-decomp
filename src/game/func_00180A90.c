#include "kf_entity.h"

int rand(void);
s32 func_001984B0(Entity *, s32);
s32 func_001992F0(Entity *, s32);
void func_00190B28(Entity *);

s32 func_00180A90(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;

    for (i = 0; i < 6; i++) {
        if (i < 3) {
            if (func_001984B0(entity, i) && (i == 1 || i == 2)) {
                chance = (entity->unkC0[i] * ((EntityUnkE0Kind17FC60 *)&entity->unkE0)->unk68[i]) >> 6;
            } else {
                chance = 0;
            }
        } else {
            if (func_001992F0(entity, i - 3) && i == 3) {
                chance = (entity->unkC0[3] * unkE0->unk68) >> 6;
            } else {
                chance = 0;
            }
        }
        chances[i] = chance;
        total += chance;
    }
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = 1;
    }
    func_00190B28(entity);
    return (s32)(entity->flags >> 57) & 1;
}
