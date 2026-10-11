#include "common.h"
#include "kf_entity.h"

/* Steps the entity four-state timer machine. */
void func_0018E9C0(Entity *entity) {
    s64 next;

    next = entity->state;
    entity->prev_state = next;
    switch ((char)next) {
    case 0:
        if (((int)((u64)entity->flags >> 33) & 1)) {
            next = 2;
            entity->state = 2;
        }
        break;
    case 1:
        if (((int)((u64)entity->flags >> 33) & 1)) {
            next = 2;
            entity->state = 2;
        } else if (entity->timer == 0) {
            entity->state = 0;
            next = 0;
        }
        entity->timer--;
        break;
    case 2:
        if (!(((int)((u64)entity->flags >> 33) & 1))) {
            entity->state = 1;
            next = 1;
            entity->timer = ENTITY_CONTROLLER(entity)->unk12;
        }
        break;
    case 3:
        entity->state = 3;
        next = 3;
        break;
    default:
        entity->state = 0;
        next = 0;
        break;
    }
    if ((char)next != entity->prev_state) {
        entity->unkA0 = 0;
    }
}
