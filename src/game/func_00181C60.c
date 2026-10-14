#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int EntityAi_StartAction(char *, void (*)(void), int);

/* The default guard action (busy bit 0x10): blends into animation 13 and holds it for
   EntityParams.unkCE frames with flag bit 35 set, which adds to the defence (func_001B2F70) and
   keeps a hit from starting a reaction. */
void EntityAi_Guard(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xD, 0, 0xA, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAG_BITS(entity)->bit35 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction((char *)entity, unkE0->move, 1);
            }
        }
        break;
    }
}
