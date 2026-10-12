#include "kf_model.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

/* Update handler of a switch that is off: used by the player, it starts turning on. */
void func_001207D0(void *arg) {
    MapObject *object;

    if (((D_0038CC74[0] >> 1) & 1) && (object = D_00420D50[0]) == arg) {
        MAP_OBJECT_BYTES(object)->step = 2;
        object->unk10 |= 0x20000;
    }
}
