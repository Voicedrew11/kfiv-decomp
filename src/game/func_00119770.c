#include "kf_model.h"

extern u8 D_0034B69C[];

int func_0011E1F0(int, int);

/* Sets flag 0x20000 of map object 0x65 once the stage flag is set. */
void func_00119770(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = (MapObject *)func_0011E1F0(map, 0x65);
        if (object != NULL) {
            object->unk10 |= 0x20000;
        }
    }
}
