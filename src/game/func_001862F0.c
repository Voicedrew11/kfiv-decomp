#include "kf_entity.h"

int rand(void);
u32 func_001125A8(u32, u32, int);

/* Applies the side effect of a hit on the entity. func_001125A8 picks which of five from the surface
   hit: 1 and 2 start the unk20 timer with a chance the entity's unkEA lowers, 3 starts it outright,
   4 takes some of the push speed unk2C, 5 takes 30 of it. */
void func_001862F0(Entity *entity, s32 unknown_a, s32 unknown_b, s32 unknown_c) {
    s32 chance;
    s32 amount;

    switch (func_001125A8(unknown_a, unknown_b, unknown_c)) {
    case 1:
        chance = 20 - entity->params->unkEA;
        if (chance <= 0) {
            return;
        }
        if (rand() % 100 >= chance) {
            return;
        }
        entity->unk20 = 1800;
        break;
    case 2:
        chance = 50 - entity->params->unkEA;
        if (chance <= 0) {
            return;
        }
        if (rand() % 100 >= chance) {
            return;
        }
        entity->unk20 = 1800;
        break;
    case 3:
        entity->unk20 = 1800;
        break;
    case 4:
        chance = 50 - (200 - (entity->params->unkE8 + entity->params->unkDA)) / 200;
        if (chance <= 0) {
            return;
        }
        if (entity->unk2C != 0) {
            amount = entity->unk2C;
            if (chance < amount) {
                amount = chance;
            }
            entity->unk2C -= amount;
            entity->flags &= ~0xC000000000L;
            entity->unkA3 = 3;
        }
        break;
    case 5:
        if (entity->unk2C != 0) {
            entity->unk2C = (entity->unk2C > 30) ? entity->unk2C - 30 : 0;
            entity->unkA3 = 3;
            entity->flags &= ~0xC000000000L;
        }
        break;
    }
}
