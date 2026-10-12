#include "kf_entity.h"

int rand(void);
s32 func_001984B0(Entity *, s32);
s32 func_001992F0(Entity *, s32);
void func_00190B28(Entity *);

/* Picks the next attack by weighted chance: three melee attacks (EntityParams.unk240), then the
   three ranged ones, each only if its test passes; the choice goes into flag bits 57-59. */
s32 func_00164C88(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 i;
    s32 roll;
    s32 sum;
    s32 chance;
    s32 slot;

    for (i = 0; i < 6; i++) {
        if (i < 3) {
            slot = i;
            if (func_001984B0(entity, slot)) {
                chance = (entity->unkC0[i] * entity->params->unk240[slot].unk00) >> 6;
            } else {
                chance = 0;
            }
        } else if (entity->unkA2 != 0) {
            chance = 0;
        } else {
            slot = i - 3;
            if (!func_001992F0(entity, slot)) {
                chance = 0;
            } else {
                chance = (entity->unkC0[i] * ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[slot].record.unk00) >> 6;
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
