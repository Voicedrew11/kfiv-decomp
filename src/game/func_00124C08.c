#include "kf_entity.h"

extern Entity *D_004470AC;
extern int D_004470B8;

/* Unlinks an entity from its list and returns the next one. */
Entity *func_00124C08(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (D_004470A8 == entity) {
        D_004470A8 = next;
    }
    if (D_004470AC == entity) {
        D_004470AC = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    D_004470B8--;
    return next;
}
