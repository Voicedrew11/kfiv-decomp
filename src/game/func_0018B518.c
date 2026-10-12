#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"

extern int D_00446770;
extern MapSlot D_0038CD20[2];
extern ModelInstanceBlock *D_00432224[];
extern f32 D_00446850[];

int func_001C2D08(void *);
void func_001C0A88(void *, void *, void *, f32 *);

/* how far above `point` the ground of the loaded maps is (D_00446850[0]: none found) */
f32 func_0018B518(f32 *point) {
    MapSlot *slot;
    MapRecord *record;
    ModelInstanceBlock *model;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 strips_left;
    s32 points_left;

    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &D_0038CD20[0] : &D_0038CD20[1];
        if (slot->unk00 != 0) {
            record = (MapRecord *)slot->records;
            record_count = ((MapSlotUnk08 *)slot->unk08)->record_count;
            model = D_00432224[D_00446770 == map ? 0 : 22];
            for (i = 0; i < record_count; i++, model++, record++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (point[0] < record->unk60[0] || record->unk70[0] < point[0] || point[2] < record->unk60[2] ||
                    record->unk70[2] < point[2]) {
                    continue;
                }
                // match: called as void, or the mesh pointer does not take $2
                ((void (*)(void *))func_001C2D08)(model);
                mesh = (CollisionMesh *)model->model.strips;
                strips_left = mesh->unk42;
                strip = mesh->unk2C;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_001C0A88(strip_point[0].position, strip_point[1].position, strip_point[2].position, point);
                        if (COLLISION_RESULT->hit != 0) {
                            return COLLISION_RESULT->point[1] - point[1];
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    return D_00446850[0];
}
