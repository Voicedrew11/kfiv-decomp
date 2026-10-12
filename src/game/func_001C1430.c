#include "kf_map.h"
#include "kf_collision.h"

extern int D_00446770;
extern MapSlot D_0038CD20[2];
extern s32 D_004147C0[4];
extern const f32 D_00446990[];

void KF_Vector_Scale(float *, float *, float);
void func_00216778(void *, void *);
int func_001C2D08(void *);
void func_0020E918(f32 *, f32 *, f32 *, f32 *, f32 *, f32);
u32 func_001124E0(int, u32, u32);

/* Sweeps a point from `position_arg` along `direction` times `scale` through the loaded maps'
   records and leaves the nearest hit in the scratchpad result. Four of each mesh's strip lists
   are tested (D_004147C0 names them); a strip's triangles alternate in winding. */
void func_001C1430(f32 scale, void *position_arg, void *direction) {
    f32 step[4];
    f32 lowest[4];
    f32 highest[4];
    CollisionResultCopy best;
    f32 *position = position_arg;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 list;
    s32 strips_left;
    s32 points_left;
    s32 forward;

    KF_Vector_Scale(step, direction, scale);
    best.result.hit = 0;
    COLLISION_RESULT->hit = 0;
    best.result.distance = D_00446990[0];
    for (map = 0; map < 2; map++) {
        slot = D_0038CD20;
        if (D_00446770 != map) {
            slot++;
        }
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
                func_00216778(lowest, record->lowest);
                func_00216778(highest, record->highest);
                if (0.0f < step[0]) {
                    lowest[0] -= step[0];
                } else {
                    highest[0] -= step[0];
                }
                if (0.0f < step[1]) {
                    lowest[1] -= step[1];
                } else {
                    highest[1] -= step[1];
                }
                if (0.0f < step[2]) {
                    lowest[2] -= step[2];
                } else {
                    highest[2] -= step[2];
                }
                if (position[0] < lowest[0] || highest[0] < position[0] || position[1] < lowest[1] ||
                    highest[1] < position[1] || position[2] < lowest[2] || highest[2] < position[2]) {
                    continue;
                }
                func_001C2D08(record->model);
                for (list = 0; list < 4; list++) {
                    mesh = (CollisionMeshLists *)record->model->strips;
                    strips_left = mesh->strip_counts[D_004147C0[list]];
                    strip = mesh->strip_lists[D_004147C0[list]];
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        forward = TRUE;
                        do {
                            if (forward) {
                                func_0020E918((int)strip_point[0].position, (int)strip_point[1].position,
                                              (int)strip_point[2].position, (int)position, (int)direction, scale);
                            } else {
                                func_0020E918((int)strip_point[0].position, (int)strip_point[2].position,
                                              (int)strip_point[1].position, (int)position, (int)direction, scale);
                            }
                            forward ^= 1;
                            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < best.result.distance) {
                                best = *COLLISION_RESULT_COPY;
                                best.result.unk18 = func_001124E0(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
            }
        }
    }
    // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
    *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
    *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
    *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
    *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
    *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
    *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
}
