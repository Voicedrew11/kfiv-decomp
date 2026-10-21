/* GiantLeader: the behaviour of entity type 63 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Giant Leader. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "map/map.h"
#include "map_object/map_object.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_collision.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "player/player.h"
#include "entity/entity_math.h"

/* action_flags one-shot bits: that map object has been broken by this giant leader */
#define GIANT_LEADER_BREAK_FIRST  1
#define GIANT_LEADER_BREAK_SECOND 2
#define GIANT_LEADER_BREAK_THIRD  4

/* ai_state->step_index: the map object GiantLeader_BreakMapObject goes past (0xA0, 0xA1, 0xA2) */
#define GIANT_LEADER_TARGET_THIRD             1       /* map object 0xA2 */
#define GIANT_LEADER_TARGET_FIRST             2       /* map object 0xA0 */
#define GIANT_LEADER_TARGET_SECOND            3       /* map object 0xA1 */
/* GiantLeader_BreakMapObject: Entity.action_step (steps 1 and 2 are ACTION_STEP_BLEND_IN and ACTION_STEP_PLAY) */
#define GIANT_LEADER_BREAK_STEP_WALK_BLEND_IN 3       /* waits for the walk's blend-in to end */
#define GIANT_LEADER_BREAK_STEP_WALK_TO_POINT 4       /* walks to target_point, then ends the action */

static void GiantLeader_Think(Entity *entity, s32 run_now);
static void GiantLeader_ThinkCombat(Entity *entity, s32 run_now);
static void GiantLeader_MeleeAttack0(Entity *entity);
static void GiantLeader_BreakMapObject(Entity *entity);
static void GiantLeader_OnHit(Entity *entity);
static void GiantLeader_MeleeAttack1(Entity *entity);
static s32 GiantLeader_CheckPartHit(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle);

/* Set-up of a type 63 entity: clears its work area and installs the type's handlers. */
void GiantLeader_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = GiantLeader_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = GiantLeader_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
}

/* The think handler: as EntityAi_Think, with GiantLeader_ThinkCombat for state 2. Every tenth
   frame, while ai_state.step_index is 0, it also looks whether one of the map objects 0xA0 to 0xA2 lies
   between the entity and the player (EntityCollision_LineHits), with the entity and the player on two given
   map records and one of them near the object; if so it puts the number of the object in ai_state.step_index,
   marks it in ai_state.action_flags (each object is used once) and starts GiantLeader_BreakMapObject. */
static void GiantLeader_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 position[4];
    f32 target[4];
    f32 delta[4];
    Entity *first;
    Entity *second;
    Entity *third;
    MapRecord *records;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
        Vector_Copy(position, entity->model->position);
        Vector_Copy(target, &player_state);
        position[1] = entity->model->position[1] - entity->collision_height * 0.5f;
        target[1] = player_state.position.y - 115.0f;
        Vector_Subtract(delta, target, position);
        Vector_Normalize(delta, delta);
        EntityMath_Distance(position, target);
        first = (Entity *)MapObject_GetById(entity->group_id, 0xA0);
        second = (Entity *)MapObject_GetById(entity->group_id, 0xA1);
        third = (Entity *)MapObject_GetById(entity->group_id, 0xA2);
        if (ai_state->step_index == 0) {
            records = (MapRecord *)map_slots[entity->group_id].records;
            if (entity->map_record == &records[0x11]) {
                if (EntityCollision_LineHits(first, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_FIRST)
                    && (EntityMath_DistanceXZ(&player_state.position.x, first->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x14) {
                    ai_state->step_index = GIANT_LEADER_TARGET_FIRST;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_FIRST;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
                if (EntityCollision_LineHits(second, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_SECOND)
                    && (EntityMath_DistanceXZ(&player_state.position.x, second->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x16) {
                    ai_state->step_index = GIANT_LEADER_TARGET_SECOND;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_SECOND;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
            } else if (entity->map_record == &records[0x14]) {
                if (EntityCollision_LineHits(first, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_FIRST)
                    && (EntityMath_DistanceXZ(&player_state.position.x, first->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x11) {
                    ai_state->step_index = GIANT_LEADER_TARGET_FIRST;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_FIRST;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
                if (EntityCollision_LineHits(third, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_THIRD)
                    && (EntityMath_DistanceXZ(&player_state.position.x, third->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x16) {
                    ai_state->step_index = GIANT_LEADER_TARGET_THIRD;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_THIRD;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
            } else if (entity->map_record == &records[0x16]) {
                if (EntityCollision_LineHits(second, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_SECOND)
                    && (EntityMath_DistanceXZ(&player_state.position.x, second->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x11) {
                    ai_state->step_index = GIANT_LEADER_TARGET_SECOND;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_SECOND;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
                if (EntityCollision_LineHits(third, entity->model->position, &player_state.position.x) == 1 && !(ai_state->action_flags & GIANT_LEADER_BREAK_THIRD)
                    && (EntityMath_DistanceXZ(&player_state.position.x, third->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && player_state.map_index == 0x14) {
                    ai_state->step_index = GIANT_LEADER_TARGET_THIRD;
                    ai_state->action_flags |= GIANT_LEADER_BREAK_THIRD;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, run_now);
                    return;
                }
            }
        }
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                GiantLeader_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but melee record 0 is
   GiantLeader_MeleeAttack0 and melee record 1 GiantLeader_MeleeAttack1; closing in never runs. */
static void GiantLeader_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, GiantLeader_MeleeAttack0, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                EntityAi_StartAction(entity, GiantLeader_MeleeAttack1, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > DEG_TO_RAD(30);
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The melee attack with record 0 (busy bit 1): animation 20, no turning.
   GiantLeader_CheckPartHit checks six parts: 0, 2 and 4 towards the back (the heading plus
   180 degrees), 1, 3 and 5 along the heading; each pair shares one hit flag. */
static void GiantLeader_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        ai_state->turn_rate = 0.0f;
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = GiantLeader_CheckPartHit(entity, unk240, 0, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
            if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
                ENTITY_FLAG_BITS(entity)->melee_0_hit = GiantLeader_CheckPartHit(entity, unk240, 1, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = GiantLeader_CheckPartHit(entity, unk240, 2, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
            if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
                ENTITY_FLAG_BITS(entity)->melee_1_hit = GiantLeader_CheckPartHit(entity, unk240, 3, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = GiantLeader_CheckPartHit(entity, unk240, 4, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
            if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
                ENTITY_FLAG_BITS(entity)->melee_2_hit = GiantLeader_CheckPartHit(entity, unk240, 5, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
            }
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000): goes past one of three map objects (0xA0-0xA2); ai_state.step_index
   says which. The entity is put at a fixed place next to the object, facing it, and plays
   animation 17, breaks the map object (MapObject_Break) on frame EntityParams.type_param_1, then walks in
   animation 3 to a point on the far side and clears ai_state.step_index. GiantLeader_Think starts it. */
static void GiantLeader_BreakMapObject(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    void *map_object;
    f32 coord; /* also the reach tested in step 4 */
    f32 centre;
    f32 limit;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        switch (ai_state->step_index) {
        case GIANT_LEADER_TARGET_THIRD:
            centre = 4200.0f;
            coord = (entity->model->position[0] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            entity->model->position[0] = coord;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = -2400.0f;
            entity->model->position[3] = 0.0f;
            centre = 4200.0f;
            angle = -HALF_PI;
            if (entity->model->position[0] > centre) {
                angle = HALF_PI;
            }
            entity->model->rotation[1] = angle;
            coord = (entity->model->position[0] > centre ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + centre;
            ai_state->target_point[0] = coord;
            ai_state->target_point[1] = 640.0f;
            ai_state->target_point[2] = -2400.0f;
            ai_state->target_point[3] = 0.0f;
            break;
        case GIANT_LEADER_TARGET_FIRST:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 5200.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->rotation[1] = EntityMath_WrapAngle(entity->model->position[2] > limit ? 0.0f : PI);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            ai_state->target_point[0] = 5200.0f;
            ai_state->target_point[1] = 640.0f;
            ai_state->target_point[2] = coord;
            ai_state->target_point[3] = 0.0f;
            break;
        case GIANT_LEADER_TARGET_SECOND:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 3400.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->rotation[1] = EntityMath_WrapAngle(entity->model->position[2] > limit ? 0.0f : PI);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            ai_state->target_point[0] = 3400.0f;
            ai_state->target_point[1] = 640.0f;
            ai_state->target_point[2] = coord;
            ai_state->target_point[3] = 0.0f;
            break;
        }
        Motion_Start(entity, 0x11, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (entity->model->level_tables->frame == entity->params->type_param_1) {
            switch (ai_state->step_index) {
            case GIANT_LEADER_TARGET_THIRD:
                if (entity->group_id == current_map_slot) {
                    map_object = MapObject_GetById(entity->group_id, 0xA2);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case GIANT_LEADER_TARGET_FIRST:
                if (entity->group_id == current_map_slot) {
                    map_object = MapObject_GetById(entity->group_id, 0xA0);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case GIANT_LEADER_TARGET_SECOND:
                if (entity->group_id == current_map_slot) {
                    map_object = MapObject_GetById(entity->group_id, 0xA1);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 1);
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            entity->action_step++;
        }
        break;
    case GIANT_LEADER_BREAK_STEP_WALK_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        }
        break;
    case GIANT_LEADER_BREAK_STEP_WALK_TO_POINT:
        coord = 100.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) < coord) {
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->step_index = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit forgets the target and starts a
   reaction (EntityAi_ReactionHeavy); it also clears flag bits 23 and 28, which
   GiantLeader_BreakMapObject sets. */
static void GiantLeader_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_LowerWeightsAfterHit(entity);
        entity->target = NULL;
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    EntityAi_SetState(entity, 2);
}

/* The melee attack with record 1 (busy bit 1): animation 21, as EntityAi_MeleeAttack; the first
   time part 0 has hit (flag bit 54) it calls Attack_DrainPlayerMp with EntityParams.type_param_2. */
static void GiantLeader_MeleeAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x15, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->action_counter = 0;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (ai_state->action_counter == 0 && ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            Attack_DrainPlayerMp(entity->params->type_param_2);
            ai_state->action_counter = 1;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of GiantLeader_MeleeAttack0: runs the hit check EntityAi_SendMeleeHit for one part of an attack
   record, unless the animation frames [frame_from, frame_to] lie outside the frames the part is
   active on (as EntityAi_CheckMeleeHitAt). Parts 3 to 5 take their frames from the four frame marks at
   EntityParams.action_frames instead and run as part 1, 0 and 1. */
static s32 GiantLeader_CheckPartHit(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
    if (part < 3) {
        if (unk240->hit_parts[part].first_frame == 0 ||
            (frame_from < unk240->hit_parts[part].first_frame && frame_to < unk240->hit_parts[part].first_frame) ||
            (unk240->hit_parts[part].last_frame < frame_from && unk240->hit_parts[part].last_frame < frame_to)) {
            return 0;
        }
    } else if (part == 3) {
        // match: this kind's EntityParams.action_frames is a signed frame number (lh); the shared header has it u16
        if ((s16)entity->params->action_frames == 0 ||
            (frame_from < (s16)entity->params->action_frames && frame_to < (s16)entity->params->action_frames) || // match: as above
            (entity->params->near_distance - 1 < frame_from && entity->params->near_distance - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    } else if (part == 4) {
        if (entity->params->near_distance == 0 ||
            (frame_from < entity->params->near_distance && frame_to < entity->params->near_distance) ||
            (entity->params->far_distance - 1 < frame_from && entity->params->far_distance - 1 < frame_to)) {
            return 0;
        }
        part = 0;
    } else if (part == 5) {
        if (entity->params->far_distance == 0 ||
            (frame_from < entity->params->far_distance && frame_to < entity->params->far_distance) ||
            (entity->params->type_param_0 - 1 < frame_from && entity->params->type_param_0 - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    }
    return EntityAi_SendMeleeHit(entity, unk240, part, angle);
}
