#include "kf_entity.h"
#include "kf_collision.h"

extern PlayerData D_00413F80;

void func_001857E8(f32 *, f32 *, f32, f32, f32, f32);
void func_00185A28(f32 *, f32 *, f32, f32, f32);
void func_00185E28(f32 *, f32 *, f32, f32, f32);
void func_001861F8(int, float *, float, float);
void Vector_Normalize(void *, void *);

/* keep an enemy out of the player and out of the other enemies; what pushed it is left in Entity.unkB0 */
void func_00185480(Entity *entity) {
    f32 push[4];
    Entity *other;
    s32 type_id;

    type_id = entity->spawn_info->type_id;
    if (!(type_id >= 0x14 && type_id <= 0x16) && (u16)type_id != 0x2D && (u16)type_id != 0x63 && (u16)type_id != 0x97
        && ((s32)(entity->flags >> 1) & 1) && entity->unk2C != 0 && !(entity->flags & 0x8010000)) {
        if (((EntityParamsWord *)entity->params)->unk00 & 0x8000000) {
            func_00185E28(entity->model->position, D_00413F80.position, entity->radius, 100.0f, 230.0f);
        } else {
            func_001857E8(entity->model->position, D_00413F80.position, entity->radius, entity->height, 100.0f, 230.0f);
        }
        if (COLLISION_RESULT->hit != 0) {
            entity->model->position[0] = COLLISION_RESULT->point[0];
            entity->model->position[1] = COLLISION_RESULT->point[1];
            entity->model->position[2] = COLLISION_RESULT->point[2];
            ENTITY_FLAG_BITS(entity)->bit49 = TRUE;
            if (COLLISION_RESULT->hit == 2) {
                ENTITY_FLAG_BITS(entity)->bit50 = TRUE;
            }
        }
    }
    push[0] = 0.0f;
    push[1] = 0.0f;
    push[2] = 0.0f;
    push[3] = 0.0f;
    if (!((s32)(entity->flags >> 26) & 1)) {
        for (other = D_004467E0; other != NULL; other = other->next) {
            if (!((s32)(other->flags >> 1) & 1)) {
                continue;
            }
            if (((s32)(other->flags >> 2) & 3) >= 2) {
                continue;
            }
            if (other == entity) {
                continue;
            }
            if (other->params->flags & 0x10) {
                if (((EntityParamsWord *)entity->params)->unk00 & 0x8000000) {
                    if (((EntityParamsWord *)other->params)->unk00 & 0x8000000) {
                        func_001861F8((int)entity->model->position, other->model->position, entity->radius, other->radius);
                    } else {
                        func_00185E28(entity->model->position, other->model->position, entity->radius, other->radius,
                                      other->height);
                    }
                } else {
                    if (((EntityParamsWord *)other->params)->unk00 & 0x8000000) {
                        func_00185A28(entity->model->position, other->model->position, entity->radius, entity->height,
                                      other->radius);
                    } else {
                        func_001857E8(entity->model->position, other->model->position, entity->radius, entity->height,
                                      other->radius, other->height);
                    }
                }
                if (COLLISION_RESULT->hit != 0) {
                    entity->model->position[0] = COLLISION_RESULT->point[0];
                    entity->model->position[1] = COLLISION_RESULT->point[1];
                    entity->model->position[2] = COLLISION_RESULT->point[2];
                    push[0] += COLLISION_RESULT->normal[0];
                    push[1] += COLLISION_RESULT->normal[1];
                    push[2] += COLLISION_RESULT->normal[2];
                    ENTITY_FLAG_BITS(entity)->bit48 = TRUE;
                    if (COLLISION_RESULT->hit == 2) {
                        ENTITY_FLAG_BITS(entity)->bit50 = TRUE;
                    }
                }
            }
        }
    }
    Vector_Normalize(push, push);
    entity->unkB0[0] = push[0];
    entity->unkB0[2] = entity->unkB0[1] = push[2];
}
