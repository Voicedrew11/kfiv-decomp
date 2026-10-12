#include "kf_model.h"

extern Qword D_0043E780;
extern u32 D_0038CC74[];
extern void *D_00420D50[];

ItemNode *func_001C9340(int, int, void *, void *, void *, void *);
int func_001C9AB0(ModelNode *);
void func_001CFA70(int, int, int, int, int, int, ...);

/* Update handler: when the player uses an object that holds an item it is handed
   out (func_001C9340), else a message is shown, at most every 45 frames. */
void func_0011FAB0(MapObject *object) {
    Qword unk_value = D_0043E780;
    s32 *work = object->work;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (object->unk08->unk44 != 0 && !(object->unk10 & 0x40000)) {
            object->model->unk1B4(object->model);
            // match: it returns what func_001C9AB0 takes
            func_001C9AB0(((int (*)(int, s16, void *, void *, void *, void *))func_001C9340)(
                MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk44, (void *)object->model->get_point(object->model, 0),
                &object->model->rotation, &unk_value, object));
        } else if (work[0] <= 0) {
            func_001CFA70(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
            work[0] = 0x2D;
        }
    }
}
