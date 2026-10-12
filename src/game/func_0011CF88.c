#include "kf_map.h"

extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern MapSlot D_0038CD20[];

void func_001036B8(int, int, int);
void func_001036E8(int, int, int);
void func_00103648(int, int, u32, int);

/* Saves four flags of every object of both maps into the flag blocks of the maps. */
void func_0011CF88(void) {
    int map;
    int count;
    MapObject *object;
    int tag;
    int i;

    for (map = 0; map < 2; map++) {
        if (D_0038CD20[map].unk00 != 0) {
            count = D_004467B8[map];
            tag = D_0038CD20[map].tag;
            object = D_003A1A30[map];
            for (i = 0; i < count; i++, object++) {
                // match: each flag is narrowed to a byte first; without these four conversions the outer loop is
                // small enough for the compiler to hoist the 0xE000 object-table stride out of it
                func_001036B8(tag, i, (u8)MAP_OBJECT_BITS(object)->bit16);
                func_001036E8(tag, i, (u8)MAP_OBJECT_BITS(object)->bit17);
                func_00103648(tag, i, 0, (u8)MAP_OBJECT_BITS(object)->bit18);
                func_00103648(tag, i, 1, (u8)MAP_OBJECT_BITS(object)->bit19);
            }
        }
    }
}
