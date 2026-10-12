#include "kf_model.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

void func_001CFA70(int, int, int, int, int, int, ...);

/* Update handler: used by the player, it shows the message of its definition,
   at most every 45 frames. */
void func_00122730(MapObject *object) {
    int message;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (object->work[0] > 0) {
            return;
        }
        message = (s16)object->unk04->unk68;
        if (message == 0) {
            return;
        }
        func_001CFA70(0, 0, 0, 0, 0x2D, message);
        object->work[0] = 0x2D;
    }
}
