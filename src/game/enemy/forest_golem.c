/* Forest Golem: the behaviour of entity type 77 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. The Forest Golem: its one spawn (013.map,
   Holy Forest) says "Unable to move" when examined and takes the Forest Dew, the Golems' food. */
#include "entity/entity.h"
#include "map/map.h"
#include "entity/entity_world.h"
#include "map_object/map_object.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "input/input.h"
#include "math/vu0.h"
#include "script/script.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_events.h"
#include "model/motion.h"
#include "save/save_state.h"
#include "player/player.h"

/* Entity.action_step values of the forest golem's own actions past the shared ones */
#define FOREST_GOLEM_STEP_START_SCRIPT  2   /* on its action frame starts its map script, on the current map only */
#define FOREST_GOLEM_STEP_RUMBLE        3   /* rumbles the controller on its shake frames, ends with the motion */

/* ai_state->move_choice: walks straight at the player */
#define FOREST_GOLEM_MOVE_CHOICE_WALK_IN 1

/* EntitySpawnInfo.spawn_mode here is the spawn id of the entity it respawns: 0 is none */
#define FOREST_GOLEM_NO_PARTNER_SPAWN_ID 0

#define ENTITY_WORK_AREA_SIZE 0xA0

static void ForestGolem_ThinkStayInPlace(Entity *entity, int invoke_now);
static void ForestGolem_Think(Entity *entity, s32 run_now);
static void ForestGolem_StartScriptAndRumble(Entity *entity);
static void ForestGolem_Move(Entity *entity);
static void ForestGolem_InitByVariant(Entity *entity);
static void ForestGolem_ThinkByState(Entity *entity, s32 run_now);
static void ForestGolem_ThinkCombat(Entity *entity, s32 run_now);
static void ForestGolem_Death(Entity *entity);
static void ForestGolem_SignalMapObject(Entity *entity);
static void ForestGolem_ThinkUseMapObjectsPosX(Entity *entity, s32 run_now);
static void ForestGolem_WalkToMapObjectPosX(Entity *entity, s32 run_now);
static void ForestGolem_ThinkUseMapObjectsNegX(Entity *entity, s32 run_now);
static void ForestGolem_WalkToMapObjectNegX(Entity *entity, s32 run_now);

/* Set-up of a type 77 entity: clears its work area and installs the type's handlers. */
void ForestGolem_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind1740A8 *work = (EntityAiStateKind1740A8 *)&entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;
    s32 index;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = ENTITY_WORK_BYTES(entity);

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->animate = EntityAi_Animate;
    ai_state->move = ForestGolem_Move;
    ai_state->on_hit = NULL;
    ai_state->dormant = NULL;
    ai_state->death = NULL;
    index = entity->params->yaw_limit_deg;
    index += entity->params->pitch_limit_deg * 64;
    if (((u8 *)map_script_flags)[index] != 0) {
        ai_state->step_index = 1;
        ai_state->think = ForestGolem_ThinkStayInPlace;
    } else {
        ai_state->step_index = 0;
        ai_state->think = ForestGolem_Think;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    EntityAi_StartAction(entity, ai_state->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    work->point_x = 1600.0f;
    work->point_y = 0.0f;
    work->point_z = -1430.0f;
}

/* The think handler the set-up function installs when the byte of map_script_flags that
   EntityParams.pitch_limit_deg and unk432 index is set, and ForestGolem_StartScriptAndRumble when it ends: copies
   the position in work floats unk48 to unk50 to the model, relative to the map origin
   EntityParams.type_param_0 names when the entity is on another map than current_map_slot, sets the heading to
   0 and starts the move action. */
static void ForestGolem_ThinkStayInPlace(Entity *entity, int invoke_now) {
    EntityAiState *sub;
    ModelInstance *model;
    MapSlot *slot;
    Vector4 *origin;
    float x;
    float y;
    float z;

    sub = &entity->ai_state;
    model = ENTITY_MODEL(entity);
    x = sub->part_angle_x;
    model->position.x = x;
    y = sub->part_angle_y;
    model->position.y = y;
    z = sub->part_angle_z;
    model->position.z = z;
    if (entity->group_id != current_map_slot) {
        slot = &map_slots[current_map_slot];
        origin = &slot->origins[ENTITY_CONTROLLER(entity)->entity_group_index];
        model->position.x = x - origin->x;
        model->position.y = y - origin->y;
        model->position.z = z - origin->z;
    }
    model->rotation.y = 0.0f;
    EntityAi_StartAction(entity, sub->move, invoke_now);
}

/* The think handler (the set-up function installs it while that byte of map_script_flags is clear): while
   the entity is not busy it starts ForestGolem_StartScriptAndRumble once the byte is set, and the move
   action until then. */
static void ForestGolem_Think(Entity *entity, s32 run_now) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    if (entity->busy_flags == 0) {
        s32 index = entity->params->yaw_limit_deg;

        index += entity->params->pitch_limit_deg * 64;
        if (((u8 *)map_script_flags)[index] != 0) {
            EntityAi_StartAction(entity, ForestGolem_StartScriptAndRumble, run_now);
            return;
        }
        EntityAi_StartAction(entity, ai_state->move, run_now);
    }
}

/* An action (busy bit 0x2000) that ForestGolem_Think starts: plays animation 17 to its end. Past
   frame EntityParams.action_frames, on the map current_map_slot, it starts script EntityParams.near_distance
   (Script_Start); after that, on the four frames of forest_golem_rumble_frames and on frame 440, it asks for a
   controller rumble (Input_StartRumble) with a strength that falls off with the player's distance up to
   3000. At the end it keeps the model's position in work floats unk48 to unk50, puts
   ForestGolem_ThinkStayInPlace in the think slot and calls it. */
static void ForestGolem_StartScriptAndRumble(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    EntityFrameTable shake_frames = forest_golem_rumble_frames;
    u32 i;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case FOREST_GOLEM_STEP_START_SCRIPT:
        if ((s16)entity->params->action_frames < entity->model->level_tables->frame) { // match: read signed here
            if (entity->group_id == current_map_slot) {
                // match: an int result keeps $2 busy after the call
                Script_Start(entity->params->near_distance, 0);
                entity->action_step++;
            }
        }
        break;
    case FOREST_GOLEM_STEP_RUMBLE:
        for (i = 0; i < 4; i++) {
            if (entity->model->level_tables->frame == shake_frames.frames[i]) {
                if (entity->distance_to_player < 3000.0f) {
                    Input_StartRumble((3000.0f - entity->distance_to_player) * 64.0f / 3000.0f, 5, 0);
                }
            }
        }
        if (entity->model->level_tables->frame == 0x1B8) {
            if (entity->distance_to_player < 3000.0f) {
                Input_StartRumble((3000.0f - entity->distance_to_player) * 32.0f / 3000.0f, 0x3C, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->point_x = entity->model->position[0];
            ai_state->point_y = entity->model->position[1];
            ai_state->point_z = entity->model->position[2];
            ai_state->think = ForestGolem_ThinkStayInPlace;
            ai_state->think_flag = 1;
            ai_state->think(entity, 1);
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0, and the motion never moves the
   entity. */
static void ForestGolem_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
}

/* A set-up function nothing refers to: as ForestGolem_Init, but it installs EntityAi_OnHit, EntityAi_Move and
   ForestGolem_Death, and picks the think handler by EntitySpawnInfo.variant (1
   ForestGolem_ThinkUseMapObjectsPosX, 2 ForestGolem_ThinkUseMapObjectsNegX, else ForestGolem_ThinkByState)
   where ForestGolem_Init tests a map script flag. */
static void ForestGolem_InitByVariant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = ENTITY_WORK_BYTES(entity);

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->death = ForestGolem_Death;
    ai_state->dormant = NULL;
    if (entity->spawn_info->variant == 1) {
        ai_state->think = ForestGolem_ThinkUseMapObjectsPosX;
    } else if (entity->spawn_info->variant == 2) {
        ai_state->think = ForestGolem_ThinkUseMapObjectsNegX;
    } else {
        ai_state->think = ForestGolem_ThinkByState;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    EntityAi_StartAction(entity, ai_state->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* A think handler only ForestGolem_InitByVariant installs (for an EntitySpawnInfo.variant other than 1 and 2),
   a set-up function nothing refers to: as EntityAi_Think, with ForestGolem_ThinkCombat for state 2. The same
   as ExpeditionSoldier_Think. */
static void ForestGolem_ThinkByState(Entity *entity, s32 run_now) {
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
        ForestGolem_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2 of the think handlers ForestGolem_InitByVariant installs: as
   ExpeditionSoldier_ThinkCombat without the EntityAi_ClearTurnInPlace test. */
static void ForestGolem_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 range;
    f32 angle;
    f32 turn;
    s32 next_action;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    range = 300.0f;
    angle = DEG_TO_RAD(30);
    if (EntityAi_ShouldGuard(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->action_timer == 0 || next_action == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = next_action;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, next_action);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if ((s16)entity->params->action_frames < entity->distance_to_player) { // match: read signed here
                turn = EntityMath_AngleDifference(entity->heading_to_player, entity->model->rotation[1]);
                turn = ABS(turn);
                if (turn > DEG_TO_RAD(45)) {
                    entity->target = &player_state;
                    entity->action_timer = rand() % 30 + 60;
                    ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
                } else {
                    ai_state->move_choice = MOVE_CHOICE_NONE;
                }
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 120;
                ai_state->move_choice = FOREST_GOLEM_MOVE_CHOICE_WALK_IN;
            }
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 60;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
        side = EntityAi_TurnFromPush(entity, direction);
        switch (ai_state->move_choice) {
        case MOVE_CHOICE_CLOSE_IN:
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_TurnCommand(entity);
            }
            break;
        case FOREST_GOLEM_MOVE_CHOICE_WALK_IN:
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
            break;
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* A death action (busy bit 0x40) only ForestGolem_InitByVariant puts in the death slot, a set-up function
   nothing refers to; ForestGolem_Init leaves the slot empty. As EntityAi_Death; when the fade
   starts and EntitySpawnInfo.spawn_mode is set, it calls EntityEvent_Respawn for the entity with that spawn id
   (if its flag bit 0 is set) and puts it at this entity's position. */
static void ForestGolem_Death(Entity *entity) {
    Entity *other;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
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
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            if (entity->spawn_info->spawn_mode != FOREST_GOLEM_NO_PARTNER_SPAWN_ID) {
                other = Entity_GetBySpawnId(entity->spawn_info->spawn_mode);
                if (other != NULL && ENTITY_FLAG_BITS(other)->active_list) {
                    EntityEvent_Respawn(other);
                    Vector_Copy(other->model->position, entity->model->position);
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
   with 1 for the map object work byte unk7B names. ForestGolem_WalkToMapObjectPosX and
   ForestGolem_WalkToMapObjectNegX start it. The same as ExpeditionSoldier_SignalMapObject. */
static void ForestGolem_SignalMapObject(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (entity->model->level_tables->frame == 0x14) {
            map_object = MapObject_GetById(entity->group_id, ai_state->action_flags);
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

/* A think handler only ForestGolem_InitByVariant installs (for EntitySpawnInfo.variant 1), a set-up function
   nothing refers to: as ForestGolem_ThinkByState (with ForestGolem_ThinkCombat for state 1 too), but it
   first reads bit 17 of the two map objects EntitySpawnInfo.map_object_id and unk43 name into work bytes
   unk79 and unk7A, and while either is clear it calls ForestGolem_WalkToMapObjectPosX instead of
   stepping the states. The same as ExpeditionSoldier_ThinkUseMapObjectsPosX. */
static void ForestGolem_ThinkUseMapObjectsPosX(Entity *entity, s32 run_now) {
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
        ai_state->turn_in_place = MAP_OBJECT_IS_ON(map_object);
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
    if (map_object != NULL) {
        ai_state->step_index = MAP_OBJECT_IS_ON(map_object);
    }
    if (!ai_state->turn_in_place || !ai_state->step_index) {
        ForestGolem_WalkToMapObjectPosX(entity, run_now);
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
        ForestGolem_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* Helper of ForestGolem_ThinkUseMapObjectsPosX, which calls it in place of the state functions: attacks when
   EntityAi_CanAttack says so; else walks to a fixed point (x 950) and from there to a point next to the
   map object whose bit is clear, turns to face it and starts ForestGolem_SignalMapObject for it;
   until then it starts the move action. */
static void ForestGolem_WalkToMapObjectPosX(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 near_point[4];
    f32 far_point[4];
    f32 range;

    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    range = 100.0f;
    if (entity->model->position[0] > 1200.0f || entity->model->position[0] < 800.0f) {
        ai_state->target_point[0] = 950.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -1400.0f;
        ai_state->target_point[3] = 0.0f;
        entity->target = entity->ai_state.target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (ai_state->turn_in_place == 0) {
            near_point[0] = 900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1800.0f;
            near_point[3] = 0.0f;
            far_point[0] = 800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1800.0f;
            far_point[3] = 0.0f;
            ai_state->action_flags = entity->spawn_info->map_object_id;
        } else if (ai_state->step_index == 0) {
            near_point[0] = 900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1000.0f;
            near_point[3] = 0.0f;
            far_point[0] = 800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1000.0f;
            far_point[3] = 0.0f;
            ai_state->action_flags = entity->spawn_info->second_map_object_id;
        }
        if (EntityMath_DistanceXZ(entity->model->position, near_point) < range) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            Vector_Copy(ai_state->target_point, far_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                EntityAi_StartAction(entity, ForestGolem_SignalMapObject, run_now);
                return;
            }
        } else {
            Vector_Copy(ai_state->target_point, near_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range * 0.5f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* A think handler only ForestGolem_InitByVariant installs (for EntitySpawnInfo.variant 2), a set-up function
   nothing refers to: as ForestGolem_ThinkUseMapObjectsPosX, with ForestGolem_WalkToMapObjectNegX
   (the points at x -950 and -900). */
static void ForestGolem_ThinkUseMapObjectsNegX(Entity *entity, s32 run_now) {
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
        ai_state->turn_in_place = MAP_OBJECT_IS_ON(map_object);
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
    if (map_object != NULL) {
        ai_state->step_index = MAP_OBJECT_IS_ON(map_object);
    }
    if (!ai_state->turn_in_place || !ai_state->step_index) {
        ForestGolem_WalkToMapObjectNegX(entity, run_now);
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
        ForestGolem_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* Helper of ForestGolem_ThinkUseMapObjectsNegX: as ForestGolem_WalkToMapObjectPosX with its own points (x -950).
   */
static void ForestGolem_WalkToMapObjectNegX(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 near_point[4];
    f32 far_point[4];
    f32 range;

    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    range = 100.0f;
    if (entity->model->position[0] > -800.0f || entity->model->position[0] < -1200.0f) {
        ai_state->target_point[0] = -950.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -1400.0f;
        ai_state->target_point[3] = 0.0f;
        entity->target = entity->ai_state.target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (ai_state->turn_in_place == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1000.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1000.0f;
            far_point[3] = 0.0f;
            ai_state->action_flags = entity->spawn_info->map_object_id;
        } else if (ai_state->step_index == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1800.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1800.0f;
            far_point[3] = 0.0f;
            ai_state->action_flags = entity->spawn_info->second_map_object_id;
        }
        if (EntityMath_DistanceXZ(entity->model->position, near_point) < range) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            Vector_Copy(ai_state->target_point, far_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                EntityAi_StartAction(entity, ForestGolem_SignalMapObject, run_now);
                return;
            }
        } else {
            Vector_Copy(ai_state->target_point, near_point);
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range * 0.5f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}
