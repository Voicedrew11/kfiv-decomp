#include "kf_model.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

/* Update handler, step 3: used by the player, it starts turning off. */
void func_001228C0(MapObject *object) {
    MapObject *used = D_00420D50[0];

    if (used == object && ((D_0038CC74[0] >> 1) & 1)) {
        used->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(used)->step = 4;
    }
}
