#include "kf_collision.h"

int func_001C2D08(void *);
void Collision_RayTriangle(f32 *, f32 *, f32 *, f32 *, f32 *, f32);

/* Tests a ray (from `origin` along `direction`, no further than `reach` times it) against every triangle of
   the strips of the object's model, if the object has flag 0x10000 and the mesh unk0C is 4 or more; stops at
   the first hit. */
void func_001C2E48(MapObject *object, f32 *origin, f32 *direction, f32 reach) {
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 strips_left;
    s32 triangles_left;

    if (object->unk10 & 0x10000) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh != NULL && mesh->unk0C >= 4) {
            func_001C2D08(object->model);
            strip = mesh->strips;
            strips_left = mesh->strip_count;
            do {
                point = strip->points;
                triangles_left = strip->point_count - 2;
                do {
                    Collision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction,
                                          reach);
                    if (COLLISION_RESULT->hit != 0) {
                        return;
                    }
                    point++;
                } while (--triangles_left != 0);
                strip = (CollisionStrip *)(point + 2);
            } while (--strips_left != 0);
        }
    }
}
