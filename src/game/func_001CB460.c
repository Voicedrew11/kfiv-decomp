#include "kf_map.h"

extern MapActor D_005B6100[][MAP_ACTOR_COUNT];

/* Returns the first map actor of the slot that is not in use, or NULL. */
MapActor *func_001CB460(int slot) {
    MapActor *actor = D_005B6100[slot];
    int i;

    for (i = 0; i < MAP_ACTOR_COUNT; i++) {
        if (!(actor->flags & 1)) {
            return actor;
        }
        actor++;
    }
    return NULL;
}
