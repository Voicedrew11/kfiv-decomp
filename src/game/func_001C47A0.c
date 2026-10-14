#include "kf_collision.h"

int func_001124E0(int, int, int);
int func_001C2D08(void *);
void Collision_CapsuleTriangle(f32 *, f32 *, f32 *, f32 *, f32, f32);

/* Tests an upright capsule (`centre`, `radius`, `height`) against every triangle of the strips of the object's
   model, if the object has flag 0x10000 and the mesh unk0C is 4 or more. At the first hit it stops and puts
   func_001124E0 of the hit point's three words in COLLISION_RESULT->unk18. */
void func_001C47A0(MapObject *object, f32 *centre, f32 height, f32 radius) {
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
                    Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position, centre, radius,
                                              height);
                    if (COLLISION_RESULT->hit != 0) {
                        COLLISION_RESULT->unk18 = func_001124E0(point->unk04, point->unk08, point->unk0C);
                        return;
                    }
                    point++;
                } while (--triangles_left != 0);
                strip = (CollisionStrip *)(point + 2);
            } while (--strips_left != 0);
        }
    }
}
