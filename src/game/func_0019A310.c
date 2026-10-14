#include "kf_entity.h"

/* Sets the entity's reaction, the handler that runs in place of its action until it clears itself
   (NULL clears it), and puts the reaction's step, Entity.unkCB, back to 0; optionally runs it now. */
int EntityAi_SetReaction(Entity *entity, void (*reaction)(void), int run_now) {
    entity->unkE0.reaction = reaction;
    entity->unkCB = 0;
    if (run_now != 0) {
        if (reaction != NULL) {
            reaction();
        }
    }
    return 1;
}
