#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int EntityAi_StartAction(char *, void (*)(void), int);

/* The default dodge action (busy bit 8): plays animation 10 once, its root motion moving the
   entity, then thinks. The name is a reading, kept because the code supports it this far: the
   action sets no guard flag and checks no hit; all it does is play a motion that moves the entity
   (flag bits 11 to 13 at 1 or 2); and nearly every place that starts it or a type's own dodge does
   so on func_0018EEE0 (the player is early in a swing, near and in front; chance EntityParams.unkCA
   / 5 in 100), the test that stands next to the guard's func_0018ED98, or on a type's test that the
   player faces the entity from 200 or more away (types 48 to 51, chance unkCA / 10). The types' own
   dodges play the sideways animations 6 and 7 instead (44, 60), draw among 10, 6 and 7 (50), or
   play 29 or 30 (54). */
void EntityAi_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
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
