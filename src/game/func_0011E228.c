#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern int D_00446770;

s16 func_0011E340(char *, int);
int func_001A1C58(int, void *, void *, void *);
int func_002109C8(int, int);
int func_0011E1F0(int, int);
void func_0011E0C8(MapObject *);

/* Breaks a breakable map object that has started: step 0xF, its breaking
   animation, an effect, a sound, and the object named by its source comes into use. */
void func_0011E228(MapObject *object) {
    MapObject *other;

    if (!(MAP_OBJECT_DEF_WORD(object) & 0x800)) {
        return;
    }
    if (MAP_OBJECT_BYTES(object)->step == 0) {
        return;
    }
    if (MAP_OBJECT_BYTES(object)->step == 0xF) {
        return;
    }
    object->unk10 &= ~0x100000;
    MAP_OBJECT_BYTES(object)->step = 0xF;
    if (MAP_OBJECT_DEF_WORD(object) & 0x1000) {
        func_0011E340((char *)object, 1);
    }
    func_001A1C58(object->unk04->unk50, &object->model->position, &object->model->rotation, &D_0038CD20[MAP_OBJECT_BYTES(object)->unk00 & 1]);
    if (object->unk08->unk42 != 0 && (MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770) {
        func_002109C8(object->unk08->unk42, 0);
    }
    if (object->unk04->unk68 & 0x20000) {
        other = (MapObject *)func_0011E1F0(MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk38.bytes[3]);
        if (other != NULL) {
            func_0011E0C8(other);
        }
    }
}
