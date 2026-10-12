#include "kf_collision.h"
#include "kf_player.h"

int func_001C2D08(void *);
void func_00216778(void *, void *);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);
extern PlayerState D_00413F80;

/* Pushes `position` (an upright capsule of this height and radius) sideways out of the wall
   strips of a map object's mesh. When the object is of kind 10 and the position is the
   player's, the object the player touched is noted in the player state. */
void func_001C65F0(MapObject *object, f32 *position, f32 height, f32 radius) {
    f32 centre[4];
    f32 offset[4];
    s32 touched;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;

    touched = FALSE;
    if (object->unk10 & 0x10000) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh == NULL) {
            return;
        }
        if (mesh->unk0C < 4) {
            return;
        }
        func_001C2D08(object->model);
        func_00216778(centre, position);
        centre[1] -= 0.001f;
        if (MAP_OBJECT_DEF_KIND(object->unk04) == 10 && (PlayerState *)position == &D_00413F80
            && ((PlayerState *)position)->unk20C == 0) {
            strip = mesh->unk20;
            i = mesh->unk3C;
            while (--i != -1 && !touched) {
                point = strip->points;
                j = strip->point_count - 2;
                do {
                    func_0020E1F8((int)point[0].position, (int)point[1].position, (int)point[2].position,
                                  (int)centre, radius, height);
                    if (COLLISION_RESULT->hit != 0) {
                        touched = TRUE;
                    }
                    point++;
                } while (--j != 0 && !touched);
                strip = (CollisionStrip *)(point + 2);
            }
        }
        strip = mesh->unk18;
        i = mesh->unk38;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_0020E1F8((int)point[0].position, (int)point[1].position, (int)point[2].position,
                              (int)centre, radius, height);
                if (COLLISION_RESULT->hit != 0) {
                    offset[0] = centre[0] - COLLISION_RESULT->point[0];
                    offset[1] = 0.0f;
                    offset[2] = centre[2] - COLLISION_RESULT->point[2];
                    sceVu0Normalize(offset, offset);
                    KF_Vector_Scale(offset, offset, radius);
                    centre[0] = COLLISION_RESULT->point[0] + offset[0];
                    centre[2] = COLLISION_RESULT->point[2] + offset[2];
                    if (MAP_OBJECT_DEF_KIND(object->unk04) == 10 && (PlayerState *)position == &D_00413F80
                        && ((PlayerState *)position)->unk20C == 0) {
                        touched = TRUE;
                    }
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
        position[0] = centre[0];
        position[2] = centre[2];
        if (touched && D_00413F80.unk20C == 0) {
            D_00413F80.unk20C = (s32)object;
        }
    }
}
