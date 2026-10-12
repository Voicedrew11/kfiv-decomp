#include "kf_collision.h"

int func_001C2D08(void *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void func_001883E0(void *, void *, void *, void *, f32);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);

/* Pushes a sphere out of every triangle of the owner's mesh it touches; leaves the summed
   normals in the result and moves `centre`. */
void func_0018A628(CollisionOwner *owner, void *centre, f32 radius) {
    f32 offset[4];
    f32 position[4];
    f32 normal_sum[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 hit_floor;
    s32 hit_unk18;
    s32 hit_unk20;
    s32 i;
    s32 j;

    COLLISION_RESULT->hit = 0;
    if (owner->model->strips == NULL) {
        return;
    }
    hit_floor = FALSE;
    hit_unk18 = FALSE;
    hit_unk20 = FALSE;
    func_001C2D08(owner->model);
    mesh = (CollisionMesh *)owner->model->strips;
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    func_00216778(position, centre);
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            func_001883E0(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_floor = TRUE;
                KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                func_00216C48(normal_sum, normal_sum, offset);
                KF_Vector_Scale(offset, offset, radius);
                func_00216C48(position, COLLISION_RESULT, offset);
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
            func_001883E0(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_unk18 = TRUE;
                KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                func_00216C48(normal_sum, normal_sum, offset);
                KF_Vector_Scale(offset, offset, radius);
                func_00216C48(position, COLLISION_RESULT, offset);
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
            func_001883E0(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_unk20 = TRUE;
                KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                func_00216C48(normal_sum, normal_sum, offset);
                KF_Vector_Scale(offset, offset, radius);
                func_00216C48(position, COLLISION_RESULT, offset);
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_floor || hit_unk18 || hit_unk20) {
        func_00216778(COLLISION_RESULT->normal, normal_sum);
        func_00216778(centre, position);
        COLLISION_RESULT->hit = 2;
    }
}
