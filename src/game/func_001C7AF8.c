#include "kf_collision.h"

int func_001C2D08(void *);
void func_00216778(void *, void *);
void func_001C0C50(void *, void *, void *, void *, f32);
extern f32 D_004469AC[];    /* the largest float; an array so that it is not read through $gp */

/* Drops a ball of this radius at `position` onto the floor strips of the owner's model and
   moves it to the lowest hit. Returns TRUE if it hit one. */
s32 func_001C7AF8(CollisionOwner *owner, f32 *position, f32 radius) {
    f32 centre[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;
    f32 lowest;

    if (owner->flags & 0x10000) {
        mesh = (CollisionMesh *)owner->model->strips;
        if (mesh == NULL) {
            return FALSE;
        }
        if (mesh->unk0C < 4) {
            return FALSE;
        }
        func_001C2D08(owner->model);
        func_00216778(centre, position);
        lowest = D_004469AC[0];
        centre[1] = centre[1] - 1.0f - radius;
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001C0C50(point[0].position, point[1].position, point[2].position, centre, radius);
                if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                    lowest = COLLISION_RESULT->point[1];
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
        if (lowest < D_004469AC[0]) {
            position[1] = lowest - 1.0f;
            return TRUE;
        }
    }
    return FALSE;
}
