#include "kf_entity.h"

void func_001243E8(char);
void func_001A32A8(Entity *);

/* Frees the entities matching a group id from two lists. */
void func_001240B8(int group_id) {
    Entity *entity;
    Entity *next;
    int other; /* never assigned: retail compares against callee-saved $19 */

    entity = D_004470B0;
    while (entity != NULL) {
        if (group_id == entity->unkD3 ||
            (ENTITY_CONTROLLER(entity)->flags & 0x8000 && entity->unkD3 == other && entity->unkD3 >= 0)) {
            next = entity->next;
            func_00124CA0(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (group_id == entity->unkD3) {
            func_001A32A8(entity);
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            next = entity->next;
            func_00124D48(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }
}
