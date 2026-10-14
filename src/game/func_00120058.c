#include "kf_model.h"
#include "kf_message.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

ModelNode *func_001C9580(int);
int func_001C9AB0(ModelNode *);
/* Update handler of an open chest (kind 4, step 3): when the player uses one that still holds something it
   is handed out (func_001C9580), else "Empty" is shown, at most every 45 frames. */
void MapObject_UpdateOpenChest(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source;
    int pickup;

    if (work[1] > 0) {
        work[1]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (!(object->unk10 & 0x40000) &&
            ((source = MAP_OBJECT_WORDS(object))->unk44 != 0 || source->unk48 != 0)) {
            pickup = func_001C9580(object);
            if (pickup != 0) {
                func_001C9AB0(pickup);
            }
        } else {
            if (work[1] > 0) {
                return;
            }
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_EMPTY);
            work[1] = 0x2D;
        }
    }
}
