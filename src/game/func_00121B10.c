#include "kf_model.h"

/* Sets or clears bit 17 of a map object. */
void func_00121B10(MapObject *object, int enabled) {
    MAP_OBJECT_BITS(object)->bit17 = enabled != 0;
}
