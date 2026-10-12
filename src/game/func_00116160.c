#include "kf_model.h"

extern MapProp *D_0038CDE0[2][0xFF];

/* Returns prop prop_id of the map in the given slot, NULL when either is out of range. */
void *func_00116160(int slot, int prop_id) {
    if ((u32)slot < 2 && prop_id >= 0 && prop_id < 0x100) {
        return D_0038CDE0[slot][prop_id];
    }
    return NULL;
}
