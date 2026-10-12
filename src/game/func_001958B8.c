#include "kf_entity.h"

/* Switches the entity's handler, keeping the old one as prev_handler; optionally runs the new one now. */
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now) {
    entity->action_step = 0;
    entity->prev_handler = entity->handler;
    entity->handler = handler;
    if (run_now != 0 && handler != NULL) {
        handler(entity);
    }
    return 1;
}
