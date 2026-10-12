#include "kf_collision.h"

int func_001C2D08(void *);
u32 func_001124E0(int, u32, u32);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);

/* notes on which side of the lines from the origin to the two ends the point lies */
#define TEST_SIDES(point) \
    if ((first_x - origin->x) * ((point)[2] - origin->z) - (first_z - origin->z) * ((point)[0] - origin->x) > 0.0f) { \
        first_sides |= 1; \
    } \
    if ((second_x - origin->x) * ((point)[2] - origin->z) - (second_z - origin->z) * ((point)[0] - origin->x) < 0.0f) { \
        second_sides |= 1; \
    }

/* Tests the capsule at `origin` against every triangle of a map object's collision mesh,
   leaving out the triangles whose corners all lie outside the wedge from the origin to
   (first_x, first_z) and (second_x, second_z). Stops at the first hit and notes its surface. */
void func_001C5280(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;
    s32 second_sides;
    s32 first_sides;

    if (object->unk10 & 0x10000) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh == NULL) {
            return;
        }
        if (mesh->unk0C < 4) {
            return;
        }
        func_001C2D08(object->model);
        strip = mesh->strips;
        i = mesh->strip_count;
        do {
            j = strip->point_count - 2;
            point = strip->points;
            if (j > 0) {
                second_sides = 0;
                first_sides = 0;
                TEST_SIDES((f32 *)strip->points[0].position);
                first_sides <<= 1;
                second_sides <<= 1;
                TEST_SIDES((f32 *)strip->points[1].position);
                first_sides <<= 1;
                second_sides <<= 1;
                point += 2;
                do {
                    TEST_SIDES((f32 *)point->position);
                    if (second_sides != 7 && first_sides != 7) {
                        // match: the unsigned indices give retail's two-step address
                        func_0020E1F8((int)point[-2U].position, (int)point[-1U].position, (int)point->position,
                                      (int)origin, push, radius);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_RESULT->unk18 = func_001124E0(point->unk04, point->unk08, point->unk0C);
                            return;
                        }
                    }
                    second_sides = (second_sides << 1) & 7;
                    first_sides = (first_sides << 1) & 7;
                    point++;
                } while (--j != 0);
            } else {
                point += strip->point_count;
            }
            strip = (CollisionStrip *)point;
        } while (--i != 0);
    }
}
