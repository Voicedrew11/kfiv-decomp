#include "kf_entity.h"

extern void (*D_003BE230[])(MapObject *);

void func_0011EA80(MapObject *);
void func_001A32A8(Entity *);
void *memset(void *, int, u32);

/* Takes a map object out of use: ends its kind's state machine and clears its state. */
void func_0011E140(void *arg) {
    MapObject *object = arg;

    if (object != NULL) {
        if (MAP_OBJECT_BITS(object)->step != 0) {
            D_003BE230[object->unk04->kind](object);
        }
        func_0011EA80(object);
        func_001A32A8((Entity *)object); // match: shared prototype takes an Entity
        object->unk10 &= ~0x10000;
        MAP_OBJECT_BITS(object)->step = 0;
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
        memset(object->work, 0, sizeof(object->work));
    }
}
