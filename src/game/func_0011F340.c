#include "kf_model.h"

extern void *D_00420D50[];
extern u32 D_0038CC74[];
extern u8 D_00369830[];

void func_00123000(int);
void func_0019D490(s32, s32, f32 *, f32);
void func_001CFA70(int, int, int, int, int, int, ...);

/* Update handler of a closed door-like object: when the player uses it, it opens
   if it needs no item or the player holds the item, else a message says so. */
void func_0011F340(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    s16 item;
    f32 range;
    int message;

    if (work->timer > 0) {
        work->timer--;
    }
    if (D_00420D50[0] == object && ((D_0038CC74[0] >> 1) & 1)) {
        item = object->unk08->unk44;
        if (item != 0 && !(object->unk10 & 0x40000)) {
            if (D_00369830[item] != 0) {
                func_00123000(item);
                range = 800.0f;
                if (object->unk04->unk70 > 0.0f) {
                    range = object->unk04->unk70;
                }
                // match: it returns a value
                ((int (*)(s32, s32, f32 *, f32))func_0019D490)(0x2E, 0, &object->model->position.x, range);
                object->unk10 |= 0x40000;
            } else {
                if (work->timer > 0) {
                    return;
                }
                message = (s16)object->unk04->unk68;
                if (message == 0) {
                    message = 0x3E;
                }
                func_001CFA70(0, 0, 0, 0, 0x2D, message);
                work->timer = 0x2D;
                return;
            }
        }
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_UNK60_BITS(object)->bit0 = TRUE;
    }
}
