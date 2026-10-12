#include "kf_collision.h"

extern f32 D_00446848[];

int func_001C2D08(void *);
void func_00216778(void *, void *);
void func_00188678(void *, void *, void *, void *, void *, f32);

/* Finds the nearest triangle of the prop's mesh a ray hits. Returns which of the four strip lists
   it is in (1-4, 0: none), leaves the distance in the result and copies the point to `out_point`. */
s32 func_0018AB40(void *prop_arg, f32 *origin, f32 *direction, f32 length, s32 out_point_arg) {
    MapProp *prop = prop_arg;
    void *out_point = (void *)out_point_arg;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    f32 nearest;
    s32 kind;
    s32 i;
    s32 j;

    if (prop == NULL || prop->unk32 == 0xFFFF || prop->model == NULL || prop->model->unk110 == NULL) {
        return 0;
    }
    kind = 0;
    // match: called as if it returned nothing (other files declare it int)
    ((void (*)(void *))func_001C2D08)(prop->model);
    mesh = prop->model->unk110;
    nearest = D_00446848[0];
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_00188678(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 1;
                if (out_point != NULL) {
                    func_00216778(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->unk18;
    i = mesh->unk38;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_00188678(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 2;
                if (out_point != NULL) {
                    func_00216778(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->unk20;
    i = mesh->unk3C;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_00188678(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 3;
                if (out_point != NULL) {
                    func_00216778(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->unk24;
    i = mesh->unk3E;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_00188678(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 4;
                if (out_point != NULL) {
                    func_00216778(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    COLLISION_RESULT->distance = nearest;
    return kind;
}
