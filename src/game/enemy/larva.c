/* Larva: the behaviour of entity type 67 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Larva. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "map/map.h"
#include "effect/effect.h"
#include "entity/entity_world.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

/* EntitySpawnInfo.spawn_mode: 0 is the normal dormant, else Larva_DormantHeldAtModelPoint */
#define LARVA_SPAWN_MODE_NORMAL 0

/* Entity.action_step values of the larva's own actions */
#define LARVA_DORMANT_STEP_HELD     1   /* held at a model point of another entity until the script moves it */
#define LARVA_DORMANT_STEP_ARC      1   /* flies along the arc until it lands on the floor */
#define LARVA_DEATH_STEP_FADE_OUT   1   /* fades the model out over action_timer, then frees it */
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void Larva_Think(Entity *entity, s32 run_now);
static void Larva_Dormant(Entity *entity);
static void Larva_Death(Entity *entity);
static void Larva_DormantHeldAtModelPoint(Entity *entity);
static void Larva_DormantMoveAlongArc(Entity *entity);
static void Larva_DeathFallAndFade(Entity *entity);

/* Set-up of a type 67 entity: clears its work area and installs the type's handlers. */
void Larva_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    MapSlotUnk10 *record;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->ai_state;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Larva_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    if (entity->spawn_info->spawn_mode != LARVA_SPAWN_MODE_NORMAL) {
        ai_state->dormant = Larva_DormantHeldAtModelPoint;
    } else {
        ai_state->dormant = Larva_Dormant;
    }
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    entity->target = NULL;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    record = &((MapSlotUnk10 *)map_slots[entity->group_id].light_blocks)[(s8)((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3]];
    record->ambient_green = 0.5f;
    record->ambient_red = 1.0f;
}

/* The think handler: as EntityAi_Think without EntityAi_CheckFarFromSpawn and the return to the spawn point;
   states 1 and 2 both have EntityAi_ThinkCombat. Every 20th frame it starts effect
   EntityParams.type_param_0 at model point 0xFF. While ai_state.sub_timer counts down
   (Larva_DormantMoveAlongArc sets it to 600) it shrinks the model's scale (EntityModel.scale) from
   the one kept in ai_state.pitch_speed to keep_min_distance towards half of it and lowers ambient_red and ambient_green of the
   entity's map record; at 0 it starts Larva_Death. */
static void Larva_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapSlotUnk10 *record;
    f32 scale;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case ENTITY_STATE_SEARCHING:
        case ENTITY_STATE_NOTICED:
            EntityAi_ThinkCombat(entity, run_now);
            break;
        case ENTITY_STATE_UNAWARE:
        default:
            EntityAi_ThinkIdle(entity, run_now);
            break;
        }
    }
    if (frame_counter % 20U == 0) {
        EntityEvent_StartEffectAtPoint((char **)entity, entity->params->type_param_0, 0xFF, (int)entity->model->rotation);
    }
    if (ai_state->sub_timer != 0) {
        ai_state->sub_timer--;
        scale = (f32)ai_state->sub_timer / 1200.0f + 0.5f;
        entity->model->scale[0] = ai_state->pitch_speed * scale;
        entity->model->scale[1] = ai_state->heading_speed * scale;
        entity->model->scale[2] = ai_state->keep_min_distance * scale;
        scale = (f32)ai_state->sub_timer / 600.0f;
        record = &((MapSlotUnk10 *)map_slots[entity->group_id].light_blocks)[(s8)((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3]];
        record->ambient_red = scale * 0.75f + 0.25f;
        record->ambient_green = scale * 0.375f + 0.125f;
        if (ai_state->sub_timer <= 0) {
            EntityAi_StartAction(entity, Larva_Death, 0);
        }
    }
}

/* The dormant action (busy bit 0x20) of a spawn whose EntitySpawnInfo.spawn_mode is 0: as
   EntityAi_Dormant, with flag bits 11 to 13 at 0 and flag bit 20 set as well while it lasts.
   Larva_DormantMoveAlongArc starts it too. */
static void Larva_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case DORMANT_STEP_SLEEP:
        if (entity->distance_to_player < (f32)entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_WAKE:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* A death action (busy bit 0x40) that Larva_Think starts when ai_state.sub_timer has run out;
   the death slot holds EntityAi_Death. Animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; it frees the entity's effects when EntityParams.respawn_effect_id is
   neither 0 nor 0xFFFF. */
static void Larva_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            if (entity->params->respawn_effect_id != 0 && entity->params->respawn_effect_id != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}

/* The dormant action of a spawn whose EntitySpawnInfo.spawn_mode is set (busy bit 0x20): holds animation
   17 and keeps the entity at the point Model_GetSkinnedBonePoint gives for 8 on the model of the entity whose
   spawn id is EntitySpawnInfo.variant, until ai_state.step_index is not 0: above 0 it starts
   Larva_DormantMoveAlongArc, below 0 Larva_DeathFallAndFade. */
static void Larva_DormantHeldAtModelPoint(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    Entity *other;
    f32 *point;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, 0, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_200000;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags |= ENTITY_FLAG_40000;
        entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
        entity->flags |= ENTITY_FLAG_400000;
        entity->flags |= ENTITY_FLAG_NO_WALL_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_SECOND_WALL_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->action_step++;
    case LARVA_DORMANT_STEP_HELD:
        if (ai_state->step_index > 0) {
            EntityAi_StartAction(entity, Larva_DormantMoveAlongArc, 1);
            return;
        }
        if (ai_state->step_index < 0) {
            EntityAi_StartAction(entity, Larva_DeathFallAndFade, 1);
            return;
        }
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && (other->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) == (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) {
            point = (f32 *)Model_GetSkinnedBonePoint((char *)other->model, 8);
            if (point != NULL) {
                entity->model->position[0] = point[0];
                entity->model->position[1] = point[1];
                entity->model->position[2] = point[2];
            }
        }
        break;
    }
}

/* A second dormant action (busy bit 0x20), which Larva_DormantHeldAtModelPoint starts: animation 18. It
   turns the entity to Entity.heading_to_player, give or take half of EntityParams.action_frames degrees, and sends it
   off at 40 degrees with a speed of 24 to 48 by the distance to the player, the y speed growing by
   EntityParams.pitch_limit_deg (limit yaw_limit_deg); it checks part 0 of melee record 0 for a hit (EntityAi_CheckMeleeHit)
   and starts effect type_param_1 every third frame. When flag bit 44 is set it puts the model's scale
   back, sets ai_state.sub_timer to 600, goes to state 2 and starts Larva_Dormant. */
static void Larva_DormantMoveAlongArc(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 max_angle;
    f32 min_angle;
    f32 half;
    f32 degree;
    f32 launch_angle;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        half = 0.5f;
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames, 1);
        degree = DEG_TO_RAD(1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
        ai_state->pitch_speed = entity->model->scale[0];
        ai_state->heading_speed = entity->model->scale[1];
        ai_state->keep_min_distance = entity->model->scale[2];
        entity->model->scale[0] = 0.75f;
        entity->model->scale[1] = 0.75f;
        entity->model->scale[2] = half;
        min_angle = 105.0f - (f32)(s16)entity->params->action_frames * half;
        max_angle = (f32)(s16)entity->params->action_frames * half + -130.0f;
        angle =
            entity->heading_to_player + ((f32)(rand() % ((s16)entity->params->action_frames + 1)) - (f32)(s16)entity->params->action_frames * half) * degree;
        entity->model->rotation[1] = angle;
        max_angle *= degree;
        min_angle *= degree;
        if (entity->model->rotation[1] < min_angle && 0.0f <= entity->model->rotation[1]) {
            entity->model->rotation[1] = min_angle;
        }
        if (max_angle < entity->model->rotation[1] && entity->model->rotation[1] < 0.0f) {
            entity->model->rotation[1] = max_angle;
        }
        ai_state->part_angle_z =
            (entity->distance_to_player + (f32)(rand() % (entity->params->near_distance + 1)) - (f32)entity->params->near_distance * 0.5f - 500.0f) /
                1300.0f * 24.0f + 24.0f;
        if (ai_state->part_angle_z < 24.0f) {
            ai_state->part_angle_z = 24.0f;
        }
        if (48.0f < ai_state->part_angle_z) {
            ai_state->part_angle_z = 48.0f;
        }
        launch_angle = DEG_TO_RAD(40);
        ai_state->part_angle_x = -ai_state->part_angle_z * sinf(launch_angle);
        ai_state->part_angle_y = ai_state->part_angle_z * cosf(launch_angle);
        ai_state->pause_timer = 0;
        ai_state->action_counter = 0;
        entity->action_step++;
        break;
    case LARVA_DORMANT_STEP_ARC:
        ai_state->part_angle_x += (f32)entity->params->pitch_limit_deg * ((f32)ai_state->pause_timer / 30.0f);
        if ((f32)entity->params->yaw_limit_deg < ai_state->part_angle_x) {
            ai_state->part_angle_x = (f32)entity->params->yaw_limit_deg;
        }
        if (frame_counter % 3U == 0) {
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->type_param_1, 0xFF, (int)entity->model->rotation);
        }
        if (ai_state->action_counter == 0) {
            ai_state->action_counter = EntityAi_CheckMeleeHit(entity, entity->params->melee_attacks, 0, 1, 2);
        }
        if (ENTITY_FLAG_BITS(entity)->on_floor) {
            ENTITY_FLAG_BITS(entity)->no_player_collision = 0;
            entity->model->rotation[0] = 0.0f;
            ai_state->sub_timer = 600;
            entity->model->scale[0] = ai_state->pitch_speed;
            entity->model->scale[1] = ai_state->heading_speed;
            entity->model->scale[2] = ai_state->keep_min_distance;
            ai_state->action_flags = 1;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->type_param_2, 0xFF, (int)entity->model->rotation);
            EntityAi_SetState(entity, 2);
            entity->flags &= ~ENTITY_FLAG_200000;
            entity->flags &= ~ENTITY_FLAG_100000;
            entity->flags &= ~ENTITY_FLAG_40000;
            entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_StartAction(entity, Larva_Dormant, 1);
            return;
        }
        break;
    }
    ai_state->pause_timer++;
    EntityMath_FacingVector((Vector4 *)direction, entity->model->rotation[1]);
    Vector_Scale(direction, direction, ai_state->part_angle_y);
    entity->move_delta_x += direction[0];
    entity->move_delta_y += ai_state->part_angle_x;
    entity->move_delta_z += direction[2];
    entity->model->rotation[0] = atan2f(ai_state->part_angle_x, ai_state->part_angle_y);
}

/* A second death action (busy bit 0x40), which Larva_DormantHeldAtModelPoint starts when
   ai_state.step_index is below 0: animation 17; fades the model out over 20 frames while the entity
   moves down (y grows downward, EntityCollision_DistanceToPropFloor) with a speed that grows by EntityParams.pitch_limit_deg
   (limit yaw_limit_deg), then frees its effects and removes it. */
static void Larva_DeathFallAndFade(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 1);
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->dying = 2;
        ENTITY_FLAG_BITS(entity)->no_respawn = 1;
        entity->action_timer = 20;
        ai_state->pause_timer = 0;
        entity->action_step++;
    case LARVA_DEATH_STEP_FADE_OUT:
        ai_state->part_angle_x += (f32)entity->params->pitch_limit_deg * ((f32)ai_state->pause_timer / 30.0f);
        if ((f32)entity->params->yaw_limit_deg < ai_state->part_angle_x) {
            ai_state->part_angle_x = (f32)entity->params->yaw_limit_deg;
        }
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            if (entity->params->respawn_effect_id != 0 && entity->params->respawn_effect_id != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
    entity->move_delta_y += ai_state->part_angle_x + ai_state->part_angle_x;
    ai_state->pause_timer++;
}
