#include "kf_entity.h"

extern Entity *D_004470AC;
extern int D_004470B8;

/* Appends an entity to the tail of list A. */
void func_00124BC8(Entity *entity) {
    Entity *tail;

    if (D_004470A8 != NULL) {
        tail = D_004470AC;
        D_004470AC = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004470A8 = entity;
        D_004470AC = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    D_004470B8++;
}
