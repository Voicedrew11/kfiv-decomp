#include "kf_entity.h"

void func_0018D8B8(Entity *, s32, s32, s32);
int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
int EntityAi_SetReaction(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
int EntityAi_StartAction(char *, void (*)(void), int);

/* The reaction to a light hit (busy bit 2): plays animation 14 until it has blended in, then ends
   whatever action was running and thinks. */
void EntityAi_ReactionLight(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_00192C98(entity, 0xE, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        EntityAi_SetReaction((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction((char *)entity, unkE0->move, 1);
        }
        break;
    }
}
