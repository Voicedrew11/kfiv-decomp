#include "kf_model.h"

void func_001CFA70(int, int, int, int, int, int, ...);

/* Update handler of the message kind, step 3: shows the message for 45 frames. */
void func_00120E48(MapObject *object) {
    s32 *work = object->work;

    func_001CFA70(0, 0, 0, 0, 0x2D, work[1]);
    work[2] = 0x2D;
    MAP_OBJECT_BYTES(object)->step = 4;
}
