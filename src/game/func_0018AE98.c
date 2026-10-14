#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"
#include "kf_model.h"

extern int D_00446770;
extern MapSlot D_0038CD20[2];

void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
void Vector_Copy(void *, void *);
s32 func_0018AB40(void *, f32 *, f32 *, f32, s32);

/* Tests the ray (origin, unit direction, length) against the props of the loaded maps, the
   current map first. Returns the strip list hit (1-4, 0: none) and copies the point to `out_point`. */
s32 func_0018AE98(f32 *origin, f32 *direction, void *out_point, f32 length) {
    MapSlot *slots[2];
    f32 end[4];
    f32 point[4];
    f32 lowest[4];
    f32 highest[4];
    MapRecord *record;
    f32 nearest;
    s32 result;
    s32 kind;
    s32 map;
    s32 i;

    Vector_Scale(end, direction, length);
    Vector_Add(end, end, origin);
    if (origin[0] < end[0]) {
        lowest[0] = origin[0];
        highest[0] = end[0];
    } else {
        lowest[0] = end[0];
        highest[0] = origin[0];
    }
    if (origin[1] < end[1]) {
        lowest[1] = origin[1];
        highest[1] = end[1];
    } else {
        lowest[1] = end[1];
        highest[1] = origin[1];
    }
    if (origin[2] < end[2]) {
        lowest[2] = origin[2];
        highest[2] = end[2];
    } else {
        lowest[2] = end[2];
        highest[2] = origin[2];
    }
    nearest = 3.4028235e38f; // match: FLT_MAX as a literal (the compiler keeps it in .sdata: D_0044684C)
    result = 0;
    slots[0] = &D_0038CD20[D_00446770];
    slots[1] = &D_0038CD20[D_00446770 ^ 1];
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecord *)slots[map]->records;
            for (i = 0; i < slots[map]->header->record_count; i++, record++) {
                if (((EntitySpawnFlags *)&record->unk38)->halves[1] & 0x40000000) {
                    continue;
                }
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (((MapProp *)record)->model->strips == NULL) {
                    continue;
                }
                if (highest[0] < record->unk60[0] || record->unk70[0] < lowest[0]) {
                    continue;
                }
                if (highest[1] < record->unk60[1] || record->unk70[1] < lowest[1]) {
                    continue;
                }
                if (highest[2] < record->unk60[2] || record->unk70[2] < lowest[2]) {
                    continue;
                }
                kind = func_0018AB40(record, origin, direction, length, (s32)point);
                // match: retail never lowers `nearest`, so every hit passes and the last one wins
                if (kind != 0 && COLLISION_RESULT->distance < nearest) {
                    result = kind;
                    if (out_point != NULL) {
                        Vector_Copy(out_point, point);
                    }
                }
            }
            if (result != 0) {
                break;
            }
        }
    }
    return result;
}
