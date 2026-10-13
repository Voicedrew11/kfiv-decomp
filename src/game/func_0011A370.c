#include "kf_map_object.h"

extern u8 D_00349690[];

/* Map script: once flag D_00349690[0x1C0A] is set, sets 0x1ADB and swaps map object 0x75 for 0x76. */
void func_0011A370(int map) {
    MapObject *object;

    if (D_00349690[0x1C0A] != 0) {
        D_00349690[0x1ADB] = 1;
        object = KF_MapObject_GetById(map, 0x75);
        if (object != NULL) {
            KF_MapObject_Deactivate(object);
        }
        object = KF_MapObject_GetById(map, 0x76);
        if (object != NULL) {
            KF_MapObject_Activate(object);
        }
    }
}
