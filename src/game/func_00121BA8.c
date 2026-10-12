#include "kf_model.h"

extern void *D_00414188[];

/* Update handler, step 1: stepped on by the player, it turns on (unless its
   first wait is negative). */
void func_00121BA8(MapObject *object) {
    MapObject *stood_on = D_00414188[0];

    if (stood_on == object && MAP_OBJECT_WORDS(stood_on)->unk44 >= 0) {
        stood_on->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(stood_on)->step = 2;
    }
}
