#include "kf_model.h"

extern Qword D_0043E780;
extern u32 D_0038CC74[];
extern void *D_00420D50[];
extern u8 D_00369830[];

void *memset(void *, int, u32);
void func_001CFA70(int, int, int, int, int, int, ...);
void func_00123000(int);
void func_0019D490(s32, s32, f32 *, f32);
ItemNode *func_001C9340(int, int, void *, void *, void *, void *);
int func_001CB0D0(Vector4 *, Vector4 *, int, int, int, int, int);

/* Update handler of a chest-like kind: used by the player, it opens (when it
   needs a key the player must hold it, else a message is shown) and hands out
   its item, or spawns what its third word says. */
void func_0011FDD0(void *arg, int unused) {
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
            if (D_00369830[key] == 0) {
                if (work[1] > 0) {
                    return;
                }
                message = (s16)object->unk04->unk68;
                if (message == 0) {
                    message = 0x3E;
                }
                func_001CFA70(0, 0, 0, 0, 0x2D, message);
                work[1] = 0x2D;
                return;
            }
            func_00123000(key);
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            func_0019D490(0x2D, 0, &object->model->position.x, range);
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
            func_001CB0D0(&position, &turn, MAP_OBJECT_BYTES(object)->unk00 & 1, ((MapObjectUnk08Words *)source)->unk48, source->unk38.bytes[2], 0, 1);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        }
    }
}
