#include "kf_map.h"

void *func_001CB530(char *);
extern MapActor *D_004469E0;

/* Takes every map actor of the slot off the list of the ones in use. */
void func_001CACA0(int slot) {
    MapActor *actor = D_004469E0;

    while (actor != NULL) {
        if (((actor->flags >> 2) & 1) == slot) {
            actor = func_001CB530((char *)actor);
        } else {
            actor = actor->next;
        }
    }
}
