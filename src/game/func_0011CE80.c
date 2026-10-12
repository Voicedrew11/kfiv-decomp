#include "kf_map.h"

extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern MapSlot D_0038CD20[];

void func_001036B8(int, int, int);
void func_001036E8(int, int, int);
void func_00103648(int, int, u32, int);
void func_0011EA80(MapObject *);
void func_0011E858(int);

/* Unloads the objects of a map: saves four flags of each into the flag block
   of the map, stops their sounds, then empties the table. */
void func_0011CE80(int map) {
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int tag = D_0038CD20[map].tag;
    int i;

    for (i = 0; i < count; i++, object++) {
        func_001036B8(tag, i, MAP_OBJECT_BITS(object)->bit16);
        func_001036E8(tag, i, MAP_OBJECT_BITS(object)->bit17);
        func_00103648(tag, i, 0, MAP_OBJECT_BITS(object)->bit18);
        func_00103648(tag, i, 1, MAP_OBJECT_BITS(object)->bit19);
        func_0011EA80(object);
    }
    func_0011E858(map);
}
