#include "kf_map.h"
#include "kf_collision.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern int D_00446770;
extern MapSlot D_0038CD20[2];

void func_00216778(void *, void *);
void func_00216D20(void *, void *);
void KF_Vector_Scale(void *, void *, float);
int func_001C2D08(void *);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);

/* Keeps the player 100 units away from the wall triangles (CollisionMesh.unk18) of the records around him:
   each one that is hit pushes him straight out, level with the ground. */
void func_001BE760(void) {
    Vector4 *point = &COLLISION_SCRATCH->point;
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

    func_00216778(&COLLISION_SCRATCH->point, &D_00413F80.position);
    point->y -= 1.0f;
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
                strips_left = mesh->unk38;
                strip = mesh->unk18;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_0020E1F8((int)strip_point[0].position, (int)strip_point[1].position,
                                      (int)strip_point[2].position, (int)&COLLISION_SCRATCH->point, 100.0f, 230.0f);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_SCRATCH->push.x = COLLISION_SCRATCH->point.x - COLLISION_RESULT->point[0];
                            COLLISION_SCRATCH->push.y = 0.0f;
                            COLLISION_SCRATCH->push.z = COLLISION_SCRATCH->point.z - COLLISION_RESULT->point[2];
                            func_00216D20(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push);
                            KF_Vector_Scale(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push, 100.0f);
                            COLLISION_SCRATCH->point.x = COLLISION_RESULT->point[0] + COLLISION_SCRATCH->push.x;
                            COLLISION_SCRATCH->point.z = COLLISION_RESULT->point[2] + COLLISION_SCRATCH->push.z;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    D_00413F80.position.x = point->x;
    D_00413F80.position.z = point->z;
}
