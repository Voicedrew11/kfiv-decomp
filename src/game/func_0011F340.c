#include "kf_model.h"
#include "kf_message.h"

extern void *D_00420D50[];
extern u32 D_0038CC74[];
extern u8 item_counts[];

void MapObject_ShowKeyUsed(int);
int Sound_PlayMapAtRange(u8, u8, float *, float);
/* Update handler of a closed door (kinds 1, 2, 6 and 0x11): when the player uses it, it opens if it needs no
   item or the player holds the item (the key), else its name ("Jail Door", ...) or "Locked" is shown. */
void MapObject_UpdateClosedDoor(MapObject *object) {
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
            if (item_counts[item] != 0) {
                MapObject_ShowKeyUsed(item);
                range = 800.0f;
                if (object->unk04->unk70 > 0.0f) {
                    range = object->unk04->unk70;
                }
                // match: it returns a value
                ((int (*)(s32, s32, f32 *, f32))Sound_PlayMapAtRange)(0x2E, 0, &object->model->position.x, range);
                object->unk10 |= 0x40000;
            } else {
                if (work->timer > 0) {
                    return;
                }
                message = (s16)object->unk04->unk68;
                if (message == 0) {
                    message = MSG_LOCKED;
                }
                Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
                work->timer = 0x2D;
                return;
            }
        }
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_UNK60_BITS(object)->bit0 = TRUE;
    }
}
