/* ExpeditionSoldier: the behaviour of entity type 7 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Expedition Soldier, Lee
   Maynor. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched
   to the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "map_object/map_object.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_events.h"
#include "player/player.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

/* EntitySpawnInfo.spawn_mode: the spawn id of the partner soldier, 0 for none */
#define EXPEDITION_SOLDIER_NO_PARTNER 0
/* EntityAiState.move_choice 1 (0 is MOVE_CHOICE_CLOSE_IN): walks up to the player when far */
#define EXPEDITION_SOLDIER_MOVE_CHOICE_APPROACH 1
/* AiState map_object_42_state and map_object_43_state: 0 until the soldier has operated the map object */
#define EXPEDITION_SOLDIER_MAP_OBJECT_UNOPERATED 0
/* EntitySpawnInfo.variant: the think handler the set-up function installs */
#define EXPEDITION_SOLDIER_VARIANT_HOLD_18              1       /* holds animation 18 */
#define EXPEDITION_SOLDIER_VARIANT_HOLD_19              2       /* holds animation 19 */
#define EXPEDITION_SOLDIER_VARIANT_USE_MAP_OBJECTS_POS_X 3      /* operates the map objects on the +x side */
#define EXPEDITION_SOLDIER_VARIANT_USE_MAP_OBJECTS_NEG_X 4      /* operates the map objects on the -x side */

static void ExpeditionSoldier_ThinkHoldAnim18Or19(Entity *entity, s32 run_now);
static void ExpeditionSoldier_Think(Entity *entity, s32 run_now);
static void ExpeditionSoldier_ThinkCombat(Entity *entity, s32 run_now);
static void ExpeditionSoldier_Death(Entity *entity);
static void ExpeditionSoldier_SignalMapObject(Entity *entity);
static void ExpeditionSoldier_ThinkUseMapObjectsPosX(Entity *entity, s32 run_now);
static void ExpeditionSoldier_WalkToMapObjectPosX(Entity *entity, s32 run_now);
static void ExpeditionSoldier_ThinkUseMapObjectsNegX(Entity *entity, s32 run_now);
static void ExpeditionSoldier_WalkToMapObjectNegX(Entity *entity, s32 run_now);

/* Set-up of a type 7 entity: clears its work area and installs the type's handlers. */
void ExpeditionSoldier_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    switch (entity->spawn_info->variant) {
    case EXPEDITION_SOLDIER_VARIANT_HOLD_18:
    case EXPEDITION_SOLDIER_VARIANT_HOLD_19:
        ai_state->think = ExpeditionSoldier_ThinkHoldAnim18Or19;
        ai_state->animate = EntityAi_Animate;
        break;
    case EXPEDITION_SOLDIER_VARIANT_USE_MAP_OBJECTS_POS_X:
        ai_state->think = ExpeditionSoldier_ThinkUseMapObjectsPosX;
        ai_state->animate = EntityAi_Animate;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = EntityAi_Dormant;
        ai_state->death = ExpeditionSoldier_Death;
        break;
    case EXPEDITION_SOLDIER_VARIANT_USE_MAP_OBJECTS_NEG_X:
        ai_state->think = ExpeditionSoldier_ThinkUseMapObjectsNegX;
        ai_state->animate = EntityAi_Animate;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = EntityAi_Dormant;
        ai_state->death = ExpeditionSoldier_Death;
        break;
    default:
        ai_state->think = ExpeditionSoldier_Think;
        ai_state->animate = EntityAi_Animate;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = EntityAi_Dormant;
        ai_state->death = ExpeditionSoldier_Death;
        break;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
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

/* The think handler the set-up function installs for EntitySpawnInfo.variant 1 and 2 (it takes the
   entity only): holds animation 18 (variant 1) or 19 with flag bits 16, 18, 20 to 26 and 28 set and
   starts no action. The set-up function installs no other handler but EntityAi_Animate for these.
   */
static void ExpeditionSoldier_ThinkHoldAnim18Or19(Entity *entity, s32 run_now) {
    if (entity->spawn_info->variant == EXPEDITION_SOLDIER_VARIANT_HOLD_18) {
        Motion_Start(entity, 0x12, 0, 0, 0);
    } else {
        Motion_Start(entity, 0x13, 0, 0, 0);
    }
    entity->flags |= ENTITY_FLAG_40000;
    entity->flags |= ENTITY_FLAG_100000;
    entity->flags |= ENTITY_FLAG_200000;
    entity->flags |= ENTITY_FLAG_400000;
    entity->flags |= ENTITY_FLAG_NO_WALL_COLLISION;
    entity->flags |= ENTITY_FLAG_NO_SECOND_WALL_COLLISION;
    entity->flags |= ENTITY_FLAG_NO_MAP_OBJECT_COLLISION;
    entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
    entity->flags |= ENTITY_FLAG_NOT_TARGETABLE;
    entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
}

/* The think handler (the set-up function's default, for any EntitySpawnInfo.variant but 1 to 4): as
   EntityAi_Think, with ExpeditionSoldier_ThinkCombat for state 2. */
static void ExpeditionSoldier_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        EntityAi_ThinkIdle(entity, run_now);
        return;
    case ENTITY_STATE_SEARCHING:
        EntityAi_ThinkPursue(entity, run_now);
        return;
    case ENTITY_STATE_NOTICED:
        ExpeditionSoldier_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in from farther than EntityParams.action_frames
   only turns to the player (60 to 89 frames, when he is more than 45 degrees off the heading), and from nearer
   walks at him for 120 to 179 frames. */
static void ExpeditionSoldier_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
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
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if ((s16)entity->params->action_frames < entity->distance_to_player) {
                angle = EntityMath_AngleDifference(entity->heading_to_player, entity->model->rotation[1]);
                angle = ABS(angle);
                if (angle > DEG_TO_RAD(45)) {
                    entity->target = &player_state;
                    entity->action_timer = rand() % 30 + 0x3C;
                    ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
                } else {
                    ai_state->move_choice = MOVE_CHOICE_NONE;
                }
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 0x78;
                ai_state->move_choice = EXPEDITION_SOLDIER_MOVE_CHOICE_APPROACH;
            }
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 0x3C;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
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
            switch (ai_state->move_choice) {
            case MOVE_CHOICE_CLOSE_IN:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_TurnCommand(entity);
                }
                break;
            case EXPEDITION_SOLDIER_MOVE_CHOICE_APPROACH:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The death action (busy bit 0x40): as EntityAi_Death; when the fade starts and
   EntitySpawnInfo.spawn_mode is set, it calls EntityEvent_Respawn for the entity with that spawn id (if its
   flag bit 0 is set) and puts it at this entity's position. */
static void ExpeditionSoldier_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            if (entity->spawn_info->spawn_mode != EXPEDITION_SOLDIER_NO_PARTNER) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->spawn_mode);

                if (partner != NULL && ((s32)partner->flags & ENTITY_FLAG_ACTIVE_LIST)) {
                    EntityEvent_Respawn(partner);
                    Vector_Copy(partner->model->position, entity->model->position);
                }
            }
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* An action (busy bit 0x1000): plays animation 17 to its end; on frame 20 it calls MapObject_Signal
   with 1 for the map object work byte unk7C names. ExpeditionSoldier_WalkToMapObjectPosX and
   ExpeditionSoldier_WalkToMapObjectNegX start it. */
static void ExpeditionSoldier_SignalMapObject(Entity *entity) {
    EntityAiStateKind178358 *ai_state = (EntityAiStateKind178358 *)&entity->ai_state;
    MapObject *map_object;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (entity->model->level_tables->frame == 20) {
            map_object = MapObject_GetById(entity->group_id, ai_state->map_object_to_operate_id);
            if (map_object != NULL) {
                MapObject_Signal(map_object, 1);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
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

/* The think handler the set-up function installs for EntitySpawnInfo.variant 3: as
   ExpeditionSoldier_Think (with ExpeditionSoldier_ThinkCombat for state 1 too), but it first reads bit 17 of the two map
   objects EntitySpawnInfo.map_object_id and unk43 name into work bytes unk7A and unk7B, and while either is
   clear it calls ExpeditionSoldier_WalkToMapObjectPosX instead of stepping the states. PosX: its points
   are at x 950 and 900; ExpeditionSoldier_ThinkUseMapObjectsNegX has them at -950 and -900. */
static void ExpeditionSoldier_ThinkUseMapObjectsPosX(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->map_object_id);
    if (map_object != NULL) {
        ai_state->step_index = (map_object->state_flags >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
    if (map_object != NULL) {
        ai_state->action_flags = (map_object->state_flags >> 17) & 1;
    }
    if (!ai_state->step_index || !ai_state->action_flags) {
        ExpeditionSoldier_WalkToMapObjectPosX(entity, run_now);
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        break;
    case ENTITY_STATE_SEARCHING:
    case ENTITY_STATE_NOTICED:
        ExpeditionSoldier_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* Helper of ExpeditionSoldier_ThinkUseMapObjectsPosX, which calls it in place of the state functions: attacks when
   EntityAi_CanAttack says so; else walks to a fixed point (x 950) and from there to a point next to the
   map object whose bit is clear, turns to face it and starts ExpeditionSoldier_SignalMapObject for it; until
   then it starts the move action. */
static void ExpeditionSoldier_WalkToMapObjectPosX(Entity *entity, s32 run_now) {
    EntityAiStateKind178358 *ai_state = (EntityAiStateKind178358 *)&entity->ai_state;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    reach = 100.0f;
    if (1200.0f < entity->model->position[0] || entity->model->position[0] < 800.0f) {
        ai_state->target_point[0] = 950.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -1400.0f;
        ai_state->target_point[3] = 0.0f;
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (ai_state->map_object_42_state == EXPEDITION_SOLDIER_MAP_OBJECT_UNOPERATED) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1800.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1800.0f;
            face_point[3] = 0.0f;
            ai_state->map_object_to_operate_id = entity->spawn_info->map_object_id;
        } else if (ai_state->map_object_43_state == EXPEDITION_SOLDIER_MAP_OBJECT_UNOPERATED) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1000.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1000.0f;
            face_point[3] = 0.0f;
            ai_state->map_object_to_operate_id = entity->spawn_info->second_map_object_id;
        }
        if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            Vector_Copy(ai_state->target_point, face_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                EntityAi_StartAction(entity, ExpeditionSoldier_SignalMapObject, run_now);
                return;
            }
        } else {
            Vector_Copy(ai_state->target_point, stand_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think handler the set-up function installs for EntitySpawnInfo.variant 4: as
   ExpeditionSoldier_ThinkUseMapObjectsPosX, with ExpeditionSoldier_WalkToMapObjectNegX (the points at x -950
   and -900). */
static void ExpeditionSoldier_ThinkUseMapObjectsNegX(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->map_object_id);
    if (map_object != NULL) {
        ai_state->step_index = (map_object->state_flags >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
    if (map_object != NULL) {
        ai_state->action_flags = (map_object->state_flags >> 17) & 1;
    }
    if (!ai_state->step_index || !ai_state->action_flags) {
        ExpeditionSoldier_WalkToMapObjectNegX(entity, run_now);
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        break;
    case ENTITY_STATE_SEARCHING:
    case ENTITY_STATE_NOTICED:
        ExpeditionSoldier_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* Helper of ExpeditionSoldier_ThinkUseMapObjectsNegX: as ExpeditionSoldier_WalkToMapObjectPosX with its own points (x -950). */
static void ExpeditionSoldier_WalkToMapObjectNegX(Entity *entity, s32 run_now) {
    EntityAiStateKind178358 *ai_state = (EntityAiStateKind178358 *)&entity->ai_state;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    reach = 100.0f;
    if (-800.0f < entity->model->position[0] || entity->model->position[0] < -1200.0f) {
        ai_state->target_point[0] = -950.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -1400.0f;
        ai_state->target_point[3] = 0.0f;
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (ai_state->map_object_42_state == EXPEDITION_SOLDIER_MAP_OBJECT_UNOPERATED) {
            stand_point[0] = -900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1000.0f;
            stand_point[3] = 0.0f;
            face_point[0] = -800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1000.0f;
            face_point[3] = 0.0f;
            ai_state->map_object_to_operate_id = entity->spawn_info->map_object_id;
        } else if (ai_state->map_object_43_state == EXPEDITION_SOLDIER_MAP_OBJECT_UNOPERATED) {
            stand_point[0] = -900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1800.0f;
            stand_point[3] = 0.0f;
            face_point[0] = -800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1800.0f;
            face_point[3] = 0.0f;
            ai_state->map_object_to_operate_id = entity->spawn_info->second_map_object_id;
        }
        if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            Vector_Copy(ai_state->target_point, face_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                EntityAi_StartAction(entity, ExpeditionSoldier_SignalMapObject, run_now);
                return;
            }
        } else {
            Vector_Copy(ai_state->target_point, stand_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}
