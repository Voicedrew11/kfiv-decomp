#include "kf_model.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

/* Update handler, step 1: used by the player, it starts turning on. */
void func_00122828(MapObject *object) {
    MapObject *used = D_00420D50[0];

    if (used == object && ((D_0038CC74[0] >> 1) & 1)) {
        used->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(used)->step = 2;
    }
}
