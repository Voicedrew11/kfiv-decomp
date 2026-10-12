#include "kf_model.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

/* Update handler of the message kind at rest: used by the player, it goes to
   step 2 when its flag is set, else to step 3. */
void func_00120DC8(MapObject *object) {
    MapObject *used;

    if (((D_0038CC74[0] >> 1) & 1) && (used = D_00420D50[0]) == object) {
        if (used->work[0] != 0) {
            MAP_OBJECT_BYTES(used)->step = 2;
        } else {
            MAP_OBJECT_BYTES(used)->step = 3;
        }
    }
}
