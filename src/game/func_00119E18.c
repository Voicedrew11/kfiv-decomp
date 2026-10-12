#include "kf_collision.h"

extern u8 D_00349690[];
extern f32 D_0034C524[];

void func_00115550(MapRecordBounds *, f32 *);
void *func_00116160(int, int);

/* Puts map record 5 at the height the stage keeps for it, 590 the first time. */
void func_00119E18(int map) {
    MapRecordBounds *record;
    f32 offset[4];

    if (D_00349690[0x740] == 0) {
        D_0034C524[0] = 590.0f;
        D_00349690[0x747] = 0;
        D_00349690[0x746] = 1;
    }
    record = func_00116160(map, 5);
    if (record != NULL) {
        offset[0] = 0.0f;
        offset[1] = D_0034C524[0] - 590.0f;
        offset[2] = 0.0f;
        offset[3] = 0.0f;
        func_00115550(record, offset);
    }
}
