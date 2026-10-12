#include "kf_map.h"
#include "kf_collision.h"

extern int D_00446770;
extern MapSlot D_0038CD20[2];
extern const f32 D_00446988[];

int func_001C2D08(void *);
void func_001C0A88(void *, void *, void *, void *);
u32 func_001124E0(int, u32, u32);

/* Looks for the lowest floor of the loaded maps' records that is above `point_arg`, by no more
   than `reach`. Returns TRUE and leaves that hit in the scratchpad result when there is one. */
int func_001C0E40(f32 reach, void *point_arg) {
    CollisionResultCopy best;
    s32 found;
    f32 *point = point_arg;
    f32 lowest;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 strips_left;
    s32 points_left;

    COLLISION_RESULT->hit = 0;
    lowest = D_00446988[0];
    found = FALSE;
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &D_0038CD20[0] : &D_0038CD20[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (point[0] < record->lowest[0] || record->highest[0] < point[0] || point[2] < record->lowest[2] ||
                    record->highest[2] < point[2]) {
                    continue;
                }
                func_001C2D08(record->model);
                mesh = (CollisionMesh *)record->model->strips;
                strips_left = mesh->floor_strip_count;
                strip = mesh->floor_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_001C0A88(strip_point[0].position, strip_point[1].position, strip_point[2].position, point);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest &&
                            point[1] <= COLLISION_RESULT->point[1] && COLLISION_RESULT->point[1] <= point[1] + reach) {
                            found = TRUE;
                            COLLISION_RESULT->unk18 = func_001124E0(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                            best = *COLLISION_RESULT_COPY;
                            lowest = COLLISION_RESULT->point[1];
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    if (found) {
        // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
        *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
        *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
        *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
        *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
        *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
        *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
    }
    return found;
}
