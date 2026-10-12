#include "kf_model.h"

extern int D_004467B0[2];

int func_0011E1F0(int, int);

/* Marks the map object with this id when it is one of the kinds 3, 4 or 0x15 and
   its unk08 lies in the first 0x200 0x4000-byte blocks of its map's data. */
void func_0011E678(int map, int object_id) {
    MapObject *object = (MapObject *)func_0011E1F0(map, object_id);
    u8 kind;

    if (object != NULL) {
        kind = object->unk04->kind;
        if (((u32)(kind - 3) < 2 || kind == 0x15) &&
            ((u32)(((int)object->unk08 - D_004467B0[MAP_OBJECT_BYTES(object)->unk00 & 1]) >> 7) >> 7) < 0x200) {
            object->unk10 |= 0x40000;
        }
    }
}
