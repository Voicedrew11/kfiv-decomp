#include "kf_map.h"
#include "kf_collision.h"
#include "kf_player.h"

/* the player's position, copied to the scratchpad for the tests */
#define SCRATCH_POINT ((void *)0x70000080)

extern PlayerState D_00413F80;
extern int D_00446770;
extern MapSlot D_0038CD20[2];
extern f32 D_0044696C[];    /* the largest float; an array so that it is not read through $gp */

void func_00216778(void *, void *);
int func_001C2D08(void *);
void func_001C0C50(void *, void *, void *, void *, f32);
u32 func_001124E0(int, u32, u32);

/* Finds the lowest floor triangle within 10 units of the player in the records around him and notes what it is
   made of in PlayerState.unk210. */
void func_001BEA50(void) {
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

    lowest = D_0044696C[0];
    func_00216778(SCRATCH_POINT, &D_00413F80.position);
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &D_0038CD20[0] : &D_0038CD20[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (D_00413F80.position.x < record->lowest[0] - 100.0f
                    || record->highest[0] + 100.0f < D_00413F80.position.x
                    || D_00413F80.position.z < record->lowest[2] - 100.0f
                    || record->highest[2] + 100.0f < D_00413F80.position.z) {
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
                        func_001C0C50(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                      SCRATCH_POINT, 10.0f);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                            lowest = COLLISION_RESULT->point[1];
                            D_00413F80.unk210 = func_001124E0(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
}
