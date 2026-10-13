#include "kf_game.h"
#include "kf_map.h"
#include "kf_map_object.h"

extern MapSlot D_0038CD20[];
extern GameStatus D_0038CC50;
extern EventRunner D_00420D00;

int func_00103AE0(u8, u8, void *, u8);
int func_00103B38(u8, u8);
void func_0011F148(char *);
void func_001BC1C0(int);

/* Event handler: while an event runs on this object, moves it to step 5, takes its
   count from the map's flags, runs the animation and clears bit 17. */
void func_001221C0(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    u8 timer = work->timer;
    u8 tag = D_0038CD20[MAP_OBJECT_BYTES(object)->unk00 & 1].tag;
    int count;

    if (work->timer > 0) {
        work->timer = timer - 1;
    }
    if (D_0038CC50.bit1 && D_00420D00.current == object) {
        MAP_OBJECT_BYTES(object)->step = 5;
        count = func_00103AE0(tag, object->unk08->unk38.bytes[2], work, timer);
        func_00103B38(tag, object->unk08->unk38.bytes[2]);
        KF_MapObject_StepAnim(object);
        func_0011F148((char *)object);
        func_001BC1C0(count);
        object->unk10 &= ~0x20000;
        D_0038CC50.unk30 |= 1;
    }
}
