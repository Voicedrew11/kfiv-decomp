#include "kf_entity.h"
#include "kf_collision.h"

int func_001C2D08(void *);
u32 func_001125F0(u32, u32, int);
void func_00216C48(void *, void *, void *);
void func_001883E0(void *, void *, void *, void *, f32);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);

/* Pushes the entity's sphere (at `position`, radius unk30) out of every triangle of a model's
   collision mesh it touches, adding the wall normals to `normal_sum`. Returns TRUE if it
   touched the floor strips. */
s32 func_00187A20(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum) {
    f32 offset[4];
    s32 hit_floor;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;

    hit_floor = FALSE;
    func_001C2D08(model);
    mesh = (CollisionMesh *)model->strips;
    if (!((s32)(entity->flags >> 23) & 1)) {
        strip = mesh->unk18;
        i = mesh->unk38;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001883E0(point[0].position, point[1].position, point[2].position, position, entity->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= 0x8000L << 30;
                    KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    func_00216C48(normal_sum, normal_sum, offset);
                    KF_Vector_Scale(offset, offset, entity->unk30);
                    func_00216C48(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    if (!((s32)(entity->flags >> 24) & 1)) {
        strip = mesh->unk20;
        i = mesh->unk3C;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001883E0(point[0].position, point[1].position, point[2].position, position, entity->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= 0x8000L << 30;
                    KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    func_00216C48(normal_sum, normal_sum, offset);
                    KF_Vector_Scale(offset, offset, entity->unk30);
                    func_00216C48(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    strip = mesh->unk24;
    i = mesh->unk3E;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        if (func_001125F0(point->unk04, point->unk08, point->unk0C) >= entity->spawn_info->unk35) {
            do {
                func_001883E0(point[0].position, point[1].position, point[2].position, position, entity->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= 0x8000L << 30;
                    KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    func_00216C48(normal_sum, normal_sum, offset);
                    KF_Vector_Scale(offset, offset, entity->unk30);
                    func_00216C48(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        } else {
            strip = (CollisionStrip *)(point + strip->point_count);
        }
    }
    if (!(entity->flags & 0x4000000400000L)) {
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001883E0(point[0].position, point[1].position, point[2].position, position, entity->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    hit_floor = TRUE;
                    entity->flags |= 0x8000L << 29;
                    KF_Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    KF_Vector_Scale(offset, offset, entity->unk30);
                    func_00216C48(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    return hit_floor;
}
