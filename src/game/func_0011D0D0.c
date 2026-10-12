#include "kf_model.h"

extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern void (*D_003BE430[])(MapObject *, int);

/* Calls each object of a map through its kind's handler in D_003BE430. */
void func_0011D0D0(int map, int arg) {
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int i;

    for (i = 0; i < count; i++, object++) {
        D_003BE430[object->unk04->kind](object, arg);
    }
}
