#include "kf_entity.h"

extern Entity *D_004470B4;
extern int D_004470BC;

/* Appends an entity to the tail of list B. */
void func_00124C60(Entity *entity) {
    Entity *tail;

    if (D_004470B0 != NULL) {
        tail = D_004470B4;
        D_004470B4 = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004470B0 = entity;
        D_004470B4 = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    D_004470BC++;
}
