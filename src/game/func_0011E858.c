#include "kf_model.h"

extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject *D_00447088[2];
extern MapObject *D_00447090[2];
extern MapObject D_003A1A30[][0x200];
extern MapObject *D_00472240[][0x100];

void *memset(void *, int, u32);

/* Empties one map's objects: the two counts, the two draw lists and the tables. */
void func_0011E858(int map) {
    D_004467A8[map] = 0;
    D_004467B8[map] = 0;
    D_00447088[map] = NULL;
    D_00447090[map] = NULL;
    memset(D_003A1A30[map], 0, sizeof(D_003A1A30[map]));
    memset(D_00472240[map], 0, sizeof(D_00472240[map]));
}
