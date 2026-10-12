#include "kf_collision.h"

extern f32 D_00446844[];

int func_001C2D08(void *);
void func_001C0A88(void *, void *, void *, void *);

/* How far above `position` the lowest floor of the prop's mesh is (D_00446844 when there is none). */
f32 func_0018A9F0(void *prop_arg, void *position_arg) {
    MapProp *prop = prop_arg;
    f32 *position = position_arg;
    f32 result;
    f32 lowest;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 found;
    s32 i;
    s32 j;

    result = lowest = D_00446844[0];
    if (prop == NULL || prop->unk32 == 0xFFFF || prop->model == NULL || prop->model->strips == NULL) {
        return lowest;
    }
    found = FALSE;
    // match: called as if it returned nothing (other files declare it int)
    ((void (*)(void *))func_001C2D08)(prop->model);
    mesh = (CollisionMesh *)prop->model->strips;
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_001C0A88(point[0].position, point[1].position, point[2].position, position);
            if (COLLISION_RESULT->hit != 0) {
                f32 y = COLLISION_RESULT->point[1];

                if (position[1] <= y && y <= lowest) {
                    lowest = y;
                    found = TRUE;
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (found) {
        result = lowest - position[1];
    }
    return result;
}
