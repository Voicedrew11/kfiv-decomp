#include "kf_model.h"

void func_0019AA50(s16);

/* End handler: releases the handle the kind keeps in its work area, if any. */
void func_00120D38(MapObject *object) {
    s16 *handle = (s16 *)object->work;

    if (*handle >= 0) {
        func_0019AA50(*handle);
        *handle = -1;
    }
}
