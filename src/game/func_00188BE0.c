#include "kf_entity.h"
#include "kf_collision.h"

extern int D_00446770;
extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[];
extern MapObject D_003AFA30[];

void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void sceVu0Normalize(float *, float *);
float func_00194F80(float *, float *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void func_00189D28(CollisionOwner *, void *, f32);
void func_00189EC8(CollisionOwner *, void *, f32);
void func_0018A2B0(CollisionOwner *, void *, f32);
void func_0018A3A8(CollisionOwner *, void *, f32);
void func_0018A628(CollisionOwner *, void *, f32);

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        if (D_00446770 != map) {
            return D_003AFA30;
        }
        return D_003A1A30;
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        return D_004467B8[D_00446770 == map ? 0 : 1];
    }
    return 0;
}

/* Pushes the entity (as a sphere of radius unk30) out of the map objects it touches and adds their
   normals to its push (unkA4). */
void func_00188BE0(Entity *entity) {
    f32 position[4];
    f32 normal_sum[4];
    f32 offset[4];
    f32 *sum;
    MapObject *object;
    f32 reach;
    s32 map;
    s32 count;
    s32 i;
    s32 hit;

    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    func_00216778(position, entity->model->position);
    sum = normal_sum;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            if (object->unk14 <= 0.0f) {
                continue;
            }
            reach = entity->unk30 + object->unk14;
            if (reach < func_00194F80(position, (f32 *)&object->model->position)) {
                continue;
            }
            COLLISION_RESULT->hit = 0;
            switch (object->unk04->kind) {
            case 1:
                func_00189D28((CollisionOwner *)object, position, entity->unk30);
                break;
            case 2:
                func_00189EC8((CollisionOwner *)object, position, entity->unk30);
                break;
            case 0x11:
                func_0018A2B0((CollisionOwner *)object, position, entity->unk30);
                break;
            case 0x12:
                func_0018A3A8((CollisionOwner *)object, position, entity->unk30);
                break;
            default:
                if (object->model->strips != NULL) {
                    func_0018A628((CollisionOwner *)object, position, entity->unk30);
                }
                break;
            }
            hit = COLLISION_RESULT->hit;
            if (hit == 1) {
                Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                func_00216C48(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, entity->unk30);
                func_00216C48(position, COLLISION_RESULT, offset);
                ENTITY_FLAG_BITS(entity)->bit47 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit45 = TRUE;
            } else if (hit == 2) {
                func_00216C48(normal_sum, normal_sum, COLLISION_RESULT->normal);
                ENTITY_FLAG_BITS(entity)->bit47 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit45 = TRUE;
            }
        }
    }
    normal_sum[0] += entity->unkA4[0];
    normal_sum[1] += entity->unkA4[1];
    normal_sum[2] += entity->unkA4[2];
    sceVu0Normalize(sum, sum);
    entity->unkA4[0] = normal_sum[0];
    entity->unkA4[1] = normal_sum[1];
    entity->unkA4[2] = normal_sum[2];
    entity->model->position[0] = position[0];
    entity->model->position[1] = position[1];
    entity->model->position[2] = position[2];
}
