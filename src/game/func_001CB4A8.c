#include "kf_map.h"

extern MapActor D_005B6100[][MAP_ACTOR_COUNT];

/* Returns the oldest map actor of the slot that no map object dropped, or NULL. */
MapActor *func_001CB4A8(int slot) {
    MapActor *actor = D_005B6100[slot];
    MapActor *oldest = NULL;
    int i;

    for (i = MAP_ACTOR_COUNT - 1; i >= 0; i--) {
        if (MAP_ACTOR_WORDS(actor)->object_id == 0) {
            if (oldest == NULL) {
                oldest = actor;
            } else if (actor->serial < oldest->serial) {
                oldest = actor;
            }
        }
        actor++;
    }
    return oldest;
}
