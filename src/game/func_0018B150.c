#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"

extern int D_00446770;
extern MapSlot D_0038CD20[2];

int func_001C2D08(void *);
void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void func_00188678(void *, void *, void *, void *, void *, f32);

/* TRUE when the ray (origin, unit direction, length) hits a wall (strip list unk18) of any
   prop of the loaded maps. */
s32 func_0018B150(f32 *origin, f32 *direction, f32 length, f32 *unused) {
    f32 end[4];
    MapSlot *slot;
    MapRecord *record;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 map;
    s32 i;
    s32 strips_left;
    s32 points_left;

    Vector_Scale(end, direction, length);
    func_00216C48(end, end, origin);
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &D_0038CD20[0] : &D_0038CD20[1];
        record = (MapRecord *)slot->records;
        if (slot->unk00 != 0) {
            for (i = 0; i < slot->header->record_count; i++, record++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (origin[0] < record->unk60[0] && end[0] < record->unk60[0]) {
                    continue;
                }
                if (record->unk70[0] < origin[0] && record->unk70[0] < end[0]) {
                    continue;
                }
                if (origin[1] < record->unk60[1] && end[1] < record->unk60[1]) {
                    continue;
                }
                if (record->unk70[1] < origin[1] && record->unk70[1] < end[1]) {
                    continue;
                }
                if (origin[2] < record->unk60[2] && end[2] < record->unk60[2]) {
                    continue;
                }
                if (record->unk70[2] < origin[2] && record->unk70[2] < end[2]) {
                    continue;
                }
                // match: called as void, or the model pointer does not take $2
                ((void (*)(void *))func_001C2D08)(((MapProp *)record)->model);
                mesh = (CollisionMesh *)((MapProp *)record)->model->strips;
                strips_left = mesh->unk38;
                strip = mesh->unk18;
                while (--strips_left != -1) {
                    point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_00188678(point[0].position, point[1].position, point[2].position, origin, direction, length);
                        if (COLLISION_RESULT->hit != 0) {
                            return TRUE;
                        }
                        point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&point[2];
                }
            }
        }
    }
    return FALSE;
}
