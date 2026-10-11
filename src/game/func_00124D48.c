#include "kf_entity.h"

extern Entity *D_004467E4;
extern int D_004470C0;

void func_00125600(Entity *entity);

/* Unlinks an entity from list C and returns the next one. */
Entity *func_00124D48(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (D_004467E0 == entity) {
        D_004467E0 = next;
    }
    if (D_004467E4 == entity) {
        D_004467E4 = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    func_00125600(entity);
    entity->flags &= ~1;
    D_004470C0--;
    return next;
}
