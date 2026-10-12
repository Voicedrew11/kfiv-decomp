#include "kf_map.h"
#include "kf_game.h"

extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern MapSlot D_0038CD20[];
extern void (*D_003BE4B0[])(MapObject *);

int func_001034A8(int);
int func_00103698(int, int, int, int);
int func_001036C8(int, int, int, int);
int func_00103668(int, int, u32);
int func_00103AE0(int, int);
MapRecordInfo *func_00115CD0(CameraState *, f32, int);
void func_0011E0C8(MapObject *);

/* Starts the objects of a map: their flags come from the flag block of the map
   when it has been visited, else from their sources; each is then put in use or
   handed to the handler of its kind in D_003BE4B0. */
void func_0011CC50(int map) {
    int i = 0;
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int tag = D_0038CD20[map].tag;
    int visited = func_001034A8(tag);
    int in_use;
    int flag;
    MapObjectUnk08 *source;
    u8 kind;

    D_004467A8[map] = 1;
    for (; i < count; i++, object++) {
        source = object->unk08;
        flag = 0;
        in_use = ((s32)(source->unk38.flags >> 49) ^ 1) & 1;
        if (visited) {
            // match: called with two arguments
            in_use = ((int (*)(int, int))func_00103698)(tag, i);
            flag = ((int (*)(int, int))func_001036C8)(tag, i);
            MAP_OBJECT_BITS(object)->bit18 = func_00103668(tag, i, 0);
            MAP_OBJECT_BITS(object)->bit19 = func_00103668(tag, i, 1);
        } else {
            kind = object->unk04->kind;
            if (kind == 8 || kind == 4 || kind == 0xD || kind == 0x10) {
                flag = ((MapObjectUnk08Words *)source)->unk52;
            }
        }
        if (func_00103AE0(tag, object->unk08->unk38.bytes[2])) {
            flag = 1;
        }
        object->unk0C = (s32)func_00115CD0((CameraState *)&object->model->position, 0.0f, MAP_OBJECT_BYTES(object)->unk00 & 1);
        if (in_use) {
            func_0011E0C8(object);
        } else {
            D_003BE4B0[object->unk04->kind](object);
        }
        MAP_OBJECT_BITS(object)->bit17 = flag;
    }
}
