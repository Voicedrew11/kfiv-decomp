#include "kf_model.h"
#include "kf_linked_point.h"



/* End handler: releases the handle the kind keeps in its work area, if any. */
void func_00120D38(MapObject *object) {
    s16 *handle = (s16 *)object->work;

    if (*handle >= 0) {
        LinkedPoint_ReleaseIndex(*handle);
        *handle = -1;
    }
}
