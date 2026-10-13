#include "kf_model.h"
#include "kf_splash.h"

/* Update handler of the message kind, step 2: back to rest once Splash_Start accepts the message. */
void func_00120E10(MapObject *object) {
    if (Splash_Start(object->work[1])) {
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}
