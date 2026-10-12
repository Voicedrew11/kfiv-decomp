#include "kf_model.h"

extern MapObject *D_00472240[][0x100];

/* Looks a map object up by its object_id (1 to 255); 0 when the object_id is out of range. */
int func_0011E1F0(int map, int object_id) {
    if ((u32)(object_id - 1) < 0xFF) {
        return (int)D_00472240[map][object_id];
    }
    return 0;
}
