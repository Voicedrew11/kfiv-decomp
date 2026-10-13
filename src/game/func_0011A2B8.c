#include "kf_map_object.h"

extern u8 D_0034B098[];

/* Map script: until flag D_0034B098[0] is set, takes map objects 0x77 and 0x76 out of
   use, giving 0x77 (when it is of kind 9) the values 0xFF, 0x80, 0x18, 0. */
void func_0011A2B8(int map) {
    MapObject *object;

    if (D_0034B098[0] == 0) {
        object = KF_MapObject_GetById(map, 0x77);
        if (object != NULL) {
            KF_MapObject_Deactivate(object);
            if (object->model != NULL && object->unk04->kind == 9) {
                MAP_OBJECT_WORDS(object)->unk44 = 0xFF;
                MAP_OBJECT_WORDS(object)->unk46 = 0x80;
                MAP_OBJECT_WORDS(object)->unk48 = 0x18;
                MAP_OBJECT_WORDS(object)->unk4A = 0;
            }
        }
        object = KF_MapObject_GetById(map, 0x76);
        if (object != NULL) {
            KF_MapObject_Deactivate(object);
        }
    }
}
