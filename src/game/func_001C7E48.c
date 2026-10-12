#include "kf_collision.h"

int func_001C2D08(void *);
void func_00216778(void *, void *);
void func_001C0C50(void *, void *, void *, void *, f32);
extern f32 D_004469B4[];    /* the largest float; an array so that it is not read through $gp */

#define SAME_POINT(point, position) \
    ((point)[0] == (position).x && (point)[1] == (position).y && (point)[2] == (position).z)

/* The test of func_001C7AF8 over again (retail has it inline here, with its own copy of the limit). */
static inline s32 drop_on_floor_strips(MapObject *object, f32 *position, f32 radius) {
    f32 centre[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 strips_left;
    s32 points_left;
    f32 lowest;

    mesh = (CollisionMesh *)object->model->strips;
    if (mesh == NULL) {
        return FALSE;
    }
    if (mesh->unk0C < 4) {
        return FALSE;
    }
    func_001C2D08(object->model);
    func_00216778(centre, position);
    lowest = D_004469B4[0];
    centre[1] = centre[1] - 1.0f - radius;
    strip = mesh->floor_strips;
    strips_left = mesh->floor_strip_count;
    while (--strips_left != -1) {
        point = strip->points;
        points_left = strip->point_count - 2;
        do {
            func_001C0C50(point[0].position, point[1].position, point[2].position, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            point++;
        } while (--points_left != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (lowest < D_004469B4[0]) {
        position[1] = lowest - 1.0f;
        return TRUE;
    }
    return FALSE;
}

/* Drops a ball of this radius at `position` onto a map object and moves it to the lowest hit:
   onto the floor strips of its mesh while it rests in animation 4, else onto the eight
   four-cornered faces its model's points make. Returns TRUE if it hit. */
s32 func_001C7E48(MapObject *object, f32 *position, f32 radius) {
    f32 centre[4];
    f32 unused[4];
    s32 i;
    f32 *point0;
    f32 *point1;
    f32 *point2;
    f32 *point3;
    f32 lowest;

    if (object->unk10 & 0x10000) {
        if (MODEL_CHANNEL(object->model)->index == object->unk04->anims[4].anim
            && MODEL_CHANNEL(object->model)->param < 2) {
            return drop_on_floor_strips(object, position, radius);
        }
        func_00216778(centre, position);
        lowest = D_004469B4[0];
        centre[1] = centre[1] - 1.0f - radius;
        for (i = 0; i < 0x20; i += 4) {
            point0 = (f32 *)object->model->get_point(object->model, i);
            point1 = (f32 *)object->model->get_point(object->model, i + 1);
            point2 = (f32 *)object->model->get_point(object->model, i + 2);
            point3 = (f32 *)object->model->get_point(object->model, i + 3);
            if (SAME_POINT(point0, object->model->position) || SAME_POINT(point1, object->model->position)
                || SAME_POINT(point2, object->model->position) || SAME_POINT(point3, object->model->position)) {
                return FALSE;
            }
            func_001C0C50(point0, point1, point2, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            func_001C0C50(point0, point2, point3, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            if (lowest < D_004469B4[0]) {
                position[1] = lowest - 1.0f;
                return TRUE;
            }
        }
    }
    return FALSE;
}
