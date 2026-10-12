#include "kf_map.h"

void func_00216C48(void *, void *, void *);
extern MapActor D_005B6100[][MAP_ACTOR_COUNT];

/* Moves every map actor of the slot that is in use by offset (the maps' origins moved). */
void func_001CACF8(int slot, f32 *offset) {
    MapActor *actor = D_005B6100[slot];
    int i;

    for (i = MAP_ACTOR_COUNT - 1; i >= 0; i--) {
        if (actor->flags & 1) {
            func_00216C48(&actor->body->position, &actor->body->position, offset);
            actor->unk14 += offset[1];
        }
        actor++;
    }
}
