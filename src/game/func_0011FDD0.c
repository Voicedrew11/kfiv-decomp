#include "kf_gold.h"
#include "kf_model.h"
#include "kf_message.h"

extern Qword D_0043E780;
extern u32 D_0038CC74[];
extern void *D_00420D50[];
extern u8 item_counts[];

void *memset(void *, int, u32);
void MapObject_ShowKeyUsed(int);
int Sound_PlayMapAtRange(u8, u8, float *, float);
ItemNode *func_001C9340(int, int, void *, void *, void *, void *);

/* Update handler of a closed chest (kind 4): used by the player, it opens (when it needs a key the player
   must hold it, else its name or "Locked" is shown) and hands out its item, or drops the gold of its third
   word. */
void MapObject_UpdateClosedChest(void *arg, int unused) {
    MapObject *object = arg;
    Vector4 position;
    Vector4 turn;
    Qword unk_value;
    s32 *work;
    MapObjectUnk08 *source;
    s16 key;
    f32 range;
    int message;

    memset(&turn, 0, sizeof(turn));
    work = object->work;
    unk_value = D_0043E780;
    if (work[1] > 0) {
        work[1]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        source = object->unk08;
        key = ((MapObjectUnk08Words *)source)->unk46;
        if (key != 0) {
            if (item_counts[key] == 0) {
                if (work[1] > 0) {
                    return;
                }
                message = (s16)object->unk04->unk68;
                if (message == 0) {
                    message = MSG_LOCKED;
                }
                Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
                work[1] = 0x2D;
                return;
            }
            MapObject_ShowKeyUsed(key);
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            Sound_PlayMapAtRange(0x2D, 0, &object->model->position.x, range);
            source = object->unk08;
        }
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_BITS(object)->bit17 = TRUE;
        work[1] = 0;
        MAP_OBJECT_BITS(object)->bit18 = TRUE;
        if (source->unk44 != 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            func_001C9340(MAP_OBJECT_BYTES(object)->unk00 & 1, source->unk44, &position, &object->model->rotation, &unk_value, object);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        } else if (((MapObjectUnk08Words *)source)->unk48 > 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            turn.y = object->model->rotation.y;
            Gold_Drop(&position, &turn, MAP_OBJECT_BYTES(object)->unk00 & 1, ((MapObjectUnk08Words *)source)->unk48, source->unk38.bytes[2], 0, 1);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        }
    }
}
