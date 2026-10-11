#include "kf_entity.h"

extern Entity *D_004467E4;
extern int D_004470C0;

/* Appends an entity to the tail of list C and sets flag bit 0. */
void func_00124CF8(Entity *entity) {
    Entity *tail;

    if (D_004467E0 != NULL) {
        tail = D_004467E4;
        D_004467E4 = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004467E0 = entity;
        D_004467E4 = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    ENTITY_FLAG_BITS(entity)->bit0 = 1;
    D_004470C0++;
}
