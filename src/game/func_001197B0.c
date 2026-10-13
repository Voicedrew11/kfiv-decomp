#include "kf_map_object.h"
#include "kf_model.h"

extern u8 D_0034B69C[];


/* Sets flag 0x20000 of map object 0x65 once the stage flag is set. */
void func_001197B0(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = KF_MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->unk10 |= 0x20000;
        }
    }
}
