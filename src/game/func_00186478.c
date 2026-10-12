#include "kf_entity.h"
#include "kf_collision.h"

extern u32 D_00446ED4;

int func_001C2D08(void *);
u32 func_001125F0(u32, u32, int);
u32 func_00112618(u32, u32, int);
u32 func_001124E0(int, u32, u32);
void func_001862F0(Entity *, s32, s32, s32);
void func_001C0A88(void *, void *, void *, void *);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);

/* tests the entity's upright capsule against the triangle at `point` and pushes `position`
   sideways out of it */
#define PUSH_OUT_OF_WALL()                                                                         \
    func_0020E1F8((int)point[0].position, (int)point[1].position, (int)point[2].position,          \
                  (int)position, entity->unk30, entity->unk34);                                    \
    if (COLLISION_RESULT->hit != 0) {                                                              \
        ENTITY_FLAG_BITS(entity)->bit45 = TRUE;                                                    \
        offset[0] = position[0] - COLLISION_RESULT->point[0];                                      \
        offset[1] = 0.0f;                                                                          \
        offset[2] = position[2] - COLLISION_RESULT->point[2];                                      \
        sceVu0Normalize(offset, offset);                                                           \
        func_00216C48(normal_sum, normal_sum, offset);                                             \
        KF_Vector_Scale(offset, offset, entity->unk30);                                            \
        position[0] = COLLISION_RESULT->point[0] + offset[0];                                      \
        position[2] = COLLISION_RESULT->point[2] + offset[2];                                      \
    }

/* Collides a walking entity with a model's collision mesh: pushes `position` out of the walls
   (adding their normals to `normal_sum`) and lowers `*floor_y` to the highest floor under it.
   Returns TRUE if it found a floor. */
s32 func_00186478(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum, f32 *floor_y) {
    f32 offset[4];
    s32 hit_floor;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    u32 period;
    s32 i;
    s32 j;

    hit_floor = FALSE;
    func_001C2D08(model);
    period = 30;
    mesh = (CollisionMesh *)model->strips;
    if (D_00446ED4 % period == entity->unkD7 % 10U) {
        strip = mesh->unk28;
        i = mesh->unk40;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001C0A88(point[0].position, point[1].position, point[2].position, entity->model->position);
                if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] <= entity->model->position[1]
                    && entity->model->position[1] - entity->unk34 <= COLLISION_RESULT->point[1]) {
                    func_001862F0(entity, point->unk04, point->unk08, point->unk0C);
                    goto walls;
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
walls:
    if (!((s32)(entity->flags >> 23) & 1)) {
        strip = mesh->unk18;
        i = mesh->unk38;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                PUSH_OUT_OF_WALL();
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
                PUSH_OUT_OF_WALL();
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    if (entity->spawn_info->type_id != 0x12) {
        strip = mesh->unk24;
        i = mesh->unk3E;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            if (func_001125F0(point->unk04, point->unk08, point->unk0C) >= entity->spawn_info->unk35) {
                if (func_00112618(point->unk04, point->unk08, point->unk0C)) {
                    do {
                        func_001C0A88(point[0].position, point[1].position, point[2].position, entity->model->position);
                        if (COLLISION_RESULT->hit != 0
                            && entity->model->position[1] - entity->params->unk42C <= COLLISION_RESULT->point[1]
                            && COLLISION_RESULT->point[1] <= *floor_y) {
                            *floor_y = COLLISION_RESULT->point[1];
                            hit_floor = TRUE;
                        }
                        point++;
                    } while (--j != 0);
                } else {
                    do {
                        PUSH_OUT_OF_WALL();
                        point++;
                    } while (--j != 0);
                }
                strip = (CollisionStrip *)(point + 2);
            } else {
                strip = (CollisionStrip *)(point + strip->point_count);
            }
        }
    }
    if (!(entity->flags & 0x4000000400000L)) {
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                func_001C0A88(point[0].position, point[1].position, point[2].position, entity->model->position);
                if (COLLISION_RESULT->hit != 0
                    && entity->model->position[1] - entity->params->unk42C <= COLLISION_RESULT->point[1]
                    && COLLISION_RESULT->point[1] <= *floor_y) {
                    hit_floor = TRUE;
                    *floor_y = COLLISION_RESULT->point[1];
                    ENTITY_FLAG_BITS(entity)->bits60 = func_001124E0(point->unk04, point->unk08, point->unk0C);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    return hit_floor;
}
