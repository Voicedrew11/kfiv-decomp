#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern Entity *D_004467E0;

float sqrtf(float);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_Subtract(void *, void *, void *);
void func_0020F0D8(void *, f32, f32, void *, f32, f32);

/* moves the player out of the body just hit: to `push` from it, but by no more than 50 a frame */
#define PUSH_PLAYER_OUT()                                                                          \
    KF_Vector_Scale(offset, COLLISION_RESULT->normal, push);                                       \
    func_00216C48(offset, COLLISION_RESULT, offset);                                               \
    KF_Vector_Subtract(offset, offset, &D_00413F80);                                               \
    length = sqrtf(offset[0] * offset[0] + offset[2] * offset[2]);                                 \
    if (50.0f < length) {                                                                          \
        offset[1] = 0.0f;                                                                          \
        KF_Vector_Scale(offset, offset, 50.0f / length);                                           \
        func_00216C48(&D_00413F80, &D_00413F80, offset);                                           \
    } else {                                                                                       \
        D_00413F80.position.x = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;   \
        D_00413F80.position.z = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;   \
    }

/* Collides the player with the entities. With `climb` set the player is first put on top of any
   entity low enough to step onto (a step higher than 16.67 is taken over several frames); then
   the player is pushed out of every entity touched. Four kinds are made of up to four bodies. */
void func_001BECA8(int climb) {
    f32 old_position[4];
    f32 position[4];
    f32 offset[4];
    s32 stepped = FALSE;
    Entity *entity;
    EntityBodyPoint *body;
    void *point;
    f32 reach;
    f32 push;
    f32 radius;
    f32 height;
    f32 top;
    f32 rise;
    f32 step;
    f32 length;
    s32 i;

    D_00413F80.unk204 = 0;
    if (climb) {
        func_00216778(old_position, &D_00413F80);
        D_00413F80.position.y += 1.0f;
        reach = D_00413F80.unk1AC + 75.0f;
        if (!((s32)(D_00413F80.unk1C8 >> 33) & 1)) {
            for (entity = D_004467E0; entity != NULL; entity = entity->next) {
                if (!((s32)(entity->flags >> 1) & 1)) {
                    continue;
                }
                if (((s32)(entity->flags >> 2) & 3) >= 2) {
                    continue;
                }
                if ((s32)(entity->flags >> 27) & 1) {
                    continue;
                }
                switch (entity->spawn_info->type_id) {
                case 0x11:
                case 0x14:
                case 0x97:
                case 0x9B:
                    continue;
                }
                func_00216778(position, entity->model->position);
                radius = entity->radius;
                if (!(((EntityParamsWord *)entity->params)->unk00 & 0x8000000)) {
                    height = entity->height;
                } else {
                    height = radius + radius;
                    position[1] += radius;
                }
                if (radius + 100.0f < entity->distance_to_player) {
                    continue;
                }
                top = position[1] - height;
                if (D_00413F80.position.y < top) {
                    continue;
                }
                if (position[1] < D_00413F80.position.y - 230.0f) {
                    continue;
                }
                if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x50) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x9F) {
                    continue;
                }
                if (reach < D_00413F80.position.y - top) {
                    continue;
                }
                D_00413F80.position.y = top;
                stepped = TRUE;
                D_00413F80.unk204 = (s32)entity;
            }
        }
        D_00413F80.position.y -= 1.0f;
        if (stepped) {
            step = 16.666666f;
            rise = old_position[1] - D_00413F80.position.y;
            if (step < rise) {
                D_00413F80.position.y = old_position[1] - step;
                D_00413F80.unk1D0 = rise - step + 5.0f;
                D_00413F80.unk1D4 = step;
                D_00413F80.unk1C8 |= 1L << 33;
            }
            D_00413F80.unk1AC = 0.0f;
            D_00413F80.unk1C8 &= ~(1L << 34);
        }
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (!((s32)(entity->flags >> 1) & 1)) {
            continue;
        }
        if (((s32)(entity->flags >> 2) & 3) >= 2) {
            continue;
        }
        if ((s32)(entity->flags >> 27) & 1) {
            continue;
        }
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            continue;
        }
        switch (entity->spawn_info->type_id) {
        case 0x11:
        case 0x14:
        case 0x97:
        case 0x9B:
            ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
            body = entity->params->unk20;
            for (i = 0; i < 4; i++, body++) {
                COLLISION_RESULT->hit = 0;
                if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x9B && i != 3) {
                    continue;
                }
                point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                func_00216778(position, point);
                if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                    radius = body->unk04 * ENTITY_MODEL(entity)->unkE0;
                    height = body->unk06 * ENTITY_MODEL(entity)->unkE4;
                } else {
                    radius = body->unk04 * ENTITY_MODEL(entity)->unkE0;
                    position[1] += radius;
                    height = radius + radius;
                }
                push = 100.0f;
                func_0020F0D8(&D_00413F80, push, 230.0f, position, radius, height);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_PLAYER_OUT();
                }
            }
            break;
        default:
            push = 100.0f;
            func_0020F0D8(&D_00413F80, push, 230.0f, entity->model->position, entity->radius, entity->height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_PLAYER_OUT();
            }
            break;
        }
    }
}
