#include "kf_entity.h"

int rand(void);

/* rand() scaled to 0-99 (the factor is 2 to the power -31) */
#define ROLL_PERCENT() ((s32)(rand() * 100.0f * 0.0000000004656613f))

/* A hit may start the entity's four ailments: each has a chance in percent in the attack,
   less the entity's resistance to it. The last one started is the one that tints the model. */
void func_001B3908(Entity *entity, EntityParamsUnk240PartUnk12 *chances) {
    if ((u16)chances->unk00[0] != 0 && ROLL_PERCENT() < (u16)chances->unk00[0] - entity->params->unkEA) {
        entity->unk20 = 1800;
        entity->unk26 = 1;
    }
    if ((u16)chances->unk00[2] != 0 && ROLL_PERCENT() < (u16)chances->unk00[2] - entity->params->unkEB) {
        entity->unk24 = 3600;
        entity->unk26 = 2;
    }
    if ((u16)chances->unk00[4] != 0 && ROLL_PERCENT() < (u16)chances->unk00[4] - entity->params->unkEC) {
        entity->unk28 = 1800;
        entity->unk26 = 3;
    }
    if ((u16)chances->unk00[5] != 0 && ROLL_PERCENT() < (u16)chances->unk00[5] - entity->params->unkED) {
        entity->unk2A = 3600;
        entity->unk26 = 4;
    }
}
