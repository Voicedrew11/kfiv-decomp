#include "kf_entity.h"

/* Switches the entity to handler (keeping the old one), sets flag bit 57 and bits 58-59, optionally runs it. */
int func_001958F8(Entity *entity, void (*handler)(Entity *entity), int run, int bit57, int bits58) {
    ENTITY_FLAG_BITS(entity)->bit57 = bit57;
    ENTITY_FLAG_BITS(entity)->bits58 = bits58;
    entity->action_step = 0;
    entity->prev_handler = entity->handler;
    entity->handler = handler;
    if (run != 0 && handler != NULL) {
        handler(entity);
    }
    return 1;
}
