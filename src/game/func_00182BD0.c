#include "kf_entity.h"

void func_00190A30(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);

/* The default on_hit handler: forgets the target and starts the reaction the hit calls for. A light
   hit (flag bits 36-37 are 1) gives EntityAi_ReactionLight, or EntityAi_ReactionRewind while flag
   bit 19 is set; a heavy one (2) gives EntityAi_ReactionHeavy. The entity then goes to state 2. */
void EntityAi_OnHit(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}
