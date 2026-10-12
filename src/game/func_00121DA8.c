#include "kf_map.h"

extern MapSlot D_0038CD20[];

int func_00103AE0(int, int);
void func_00122600(void *, int);
s16 func_0011E340(char *, int);

/* Start handler: an object that starts on stays on only while func_00103AE0
   still counts something for it. */
void func_00121DA8(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    int count;

    work->part = 2;
    work->fired = 0;
    count = 0;
    if (object->unk10 & 0x20000) {
        count = func_00103AE0(D_0038CD20[MAP_OBJECT_BYTES(object)->unk00 & 1].tag, object->unk08->unk38.bytes[2]);
        if (count <= 0) {
            object->unk10 &= ~0x20000;
        }
    }
    func_00122600(object, count);
    if (!(object->unk10 & 0x20000)) {
        func_0011E340((char *)object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}
