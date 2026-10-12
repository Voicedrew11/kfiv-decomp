#include "kf_map.h"

extern MapActor *D_004469E0;

/* Marks a map actor as in use and pushes it onto the front of the list at D_004469E0. */
void func_001CB508(MapActor *actor) {
    MapActor *head = D_004469E0;

    actor->flags |= 1;
    actor->prev = NULL;
    actor->next = head;
    if (head != NULL) {
        head->prev = actor;
    }
    D_004469E0 = actor;
}
