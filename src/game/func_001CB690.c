#include "common.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern MapActor D_005B6100[][16];
extern ObjectMark D_00362928[][16];

void *memset(void *, int, u32);

void func_001CB690(MapActor *obj) {
    u32 idx;
    u32 slot;
    MapSlot *entry;
    ObjectMark *mark;

    idx = (obj->flags >> 2) & 1;
    entry = &D_0038CD20[idx];
    slot = ((char *)obj - (char *)D_005B6100[idx]) / sizeof(MapActor);
    // match: retail adds the tag row and the slot before the table base
    mark = (ObjectMark *)((u8 *)D_00362928 + (entry->tag * sizeof(D_00362928[0]) + slot * sizeof(ObjectMark)));
    memset(mark, 0, sizeof(ObjectMark));
}
