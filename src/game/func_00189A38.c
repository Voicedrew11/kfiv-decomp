#include "kf_collision.h"

int func_001C2D08(void *);
void func_001C0A88(void *, void *, void *, f32 *);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);
void func_00195100(f32 *, f32, f32); // match: the pointer is set up first, so it is the first argument here

/* Moves a sphere against the owner's mesh: finds the floor under `centre` (no more than `step`
   above it) and pushes it out of the walls. The result's hit is 1 for a wall plus 2 for a floor. */
void func_00189A38(CollisionOwner *owner, f32 *centre, f32 step, f32 radius) {
    f32 offset[4];
    f32 position[4];
    f32 normal_sum[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    f32 floor_y;
    s32 hit_wall;
    s32 hit_floor;
    s32 i;
    s32 j;

    hit_wall = 0;
    hit_floor = 0;
    COLLISION_RESULT->hit = 0;
    func_001C2D08(owner->model);
    mesh = (CollisionMesh *)owner->model->strips;
    floor_y = 3.4028235e38f; // match: FLT_MAX as a literal (the compiler keeps it in .sdata: D_00446840)
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    func_00216778(position, centre);
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_001C0A88(point[0].position, point[1].position, point[2].position, centre);
            if (COLLISION_RESULT->hit != 0) {
                if (centre[1] - step <= COLLISION_RESULT->point[1] && COLLISION_RESULT->point[1] <= centre[1] + 10.0f
                    && COLLISION_RESULT->point[1] < floor_y) {
                    floor_y = COLLISION_RESULT->point[1];
                    hit_floor = 2;
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_floor != 0) {
        position[1] = floor_y;
    }
    position[1] -= 0.1f;
    strip = mesh->unk18;
    i = mesh->unk38;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_0020E1F8(point[0].position, point[1].position, point[2].position, position, radius, step);
            if (COLLISION_RESULT->hit != 0) {
                hit_wall = 1;
                offset[0] = position[0] - COLLISION_RESULT->point[0];
                offset[1] = 0.0f;
                offset[2] = position[2] - COLLISION_RESULT->point[2];
                sceVu0Normalize(offset, offset);
                func_00216C48(normal_sum, normal_sum, offset);
                KF_Vector_Scale(offset, offset, radius);
                position[0] = COLLISION_RESULT->point[0] + offset[0];
                position[2] = COLLISION_RESULT->point[2] + offset[2];
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_wall != 0) {
        centre[0] = position[0];
        centre[2] = position[2];
        sceVu0Normalize(normal_sum, normal_sum);
        func_00216778(COLLISION_RESULT->normal, normal_sum);
    }
    if (hit_floor != 0) {
        func_00195100(&centre[1], floor_y, 3.0f);
    }
    COLLISION_RESULT->hit = hit_wall + hit_floor;
}
