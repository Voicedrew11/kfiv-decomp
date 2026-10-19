/* WiddaArcher: the behaviour of entity type 20 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Archer Large, Widda Archer
   Small. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "entity/entity.h"
#include "common.h"
#include "map_object/map_object.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
double atan2(double, double);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
float atan2f(float, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData player_state;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_AngleXZ(float *, char *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
extern u8 map_script_flags[];
float EntityMath_DistanceXZ(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int Script_Start(int, int);
extern int current_map_slot;

static void WiddaArcher_Animate(Entity *entity);
static void WiddaArcher_PitchPartToPlayer(Entity *entity);
static void WiddaArcher_AimPartsAtTarget(Entity *entity);
static void WiddaArcher_Think(Entity *entity, s32 unknown);
static void WiddaArcher_ThinkCombat(Entity *entity, s32 unknown);
static void WiddaArcher_RangedAttack(Entity *entity);
static void WiddaArcher_ThinkUseMapObject(Entity *entity, s32 unknown);
static void WiddaArcher_WalkToMapObject(Entity *entity, s32 unknown);
static void WiddaArcher_StartMapObjectScript(Entity *entity);

/* Set-up of a type 20 entity: clears its work area and installs the type's handlers. */
void WiddaArcher_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    if (entity->spawn_info->spawn_mode != 0) {
        ai_state->think = WiddaArcher_ThinkUseMapObject;
    } else {
        ai_state->think = WiddaArcher_Think;
    }
    ai_state->animate = (void (*)(void))WiddaArcher_Animate;
    ai_state->on_hit = (void (*)(void))EntityAi_OnHit;
    ai_state->move = (void (*)(void))EntityAi_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate; during animations 23 and 24, once blended in,
   WiddaArcher_PitchPartToPlayer pitches a model part (else work float unk58 is cleared), and
   WiddaArcher_AimPartsAtTarget turns two more. */
static void WiddaArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if ((u32)((u8)entity->animation_index - 0x17) < 2 && entity->model->level_tables->blend_left == 0) {
            WiddaArcher_PitchPartToPlayer(entity);
        } else {
            ai_state->heading_speed = 0.0f;
        }
        WiddaArcher_AimPartsAtTarget(entity);
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (ENTITY_FLAG_BITS(entity)->bit9) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->bit9, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of WiddaArcher_Animate: pitches the model part EntityParams.action_frames names towards the
   player, at most 50 degrees either way (work float unk58, rate 5). */
static void WiddaArcher_PitchPartToPlayer(Entity *entity) {
    EntityController *controller;
    EntityModelPart80 *part;
    EntityAiState *ai_state;
    f32 pitch;

    controller = ENTITY_CONTROLLER(entity);
    part = &((EntityModelPart80 *)entity->model->parts)[controller->action_frames];
    ai_state = &entity->ai_state;
    if (part != NULL) {
        pitch = EntityMath_WrapAngle(atan2(player_state.position[1] - entity->model->position[1], entity->distance_to_player));
        if (0.87266463f < pitch) {
            pitch = 0.87266463f;
        } else if (pitch < -0.87266463f) {
            pitch = -0.87266463f;
        }
        EntityMath_EaseAngleToward(&ai_state->heading_speed, pitch, 5.0f);
        part->unk50[0] = ai_state->heading_speed;
    }
}

/* Helper of WiddaArcher_Animate: turns model parts 20 and 21 towards the target, pitch from the
   player's height (at most 30 degrees) and yaw from the bearing (at most 40), each part by half of
   it; the angles are work floats unk48 to unk54. Without a target or while the entity is busy it
   reads the angles of parts 30 and 31 into them instead. */
static void WiddaArcher_AimPartsAtTarget(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->parts;
    f32 pitch;
    f32 pitch_limit;
    f32 half_pitch;
    f32 yaw;
    f32 yaw_limit;
    f32 half_yaw;
    f32 yaw_to_target;

    if (entity->target == NULL || entity->busy_flags != 0) {
        ai_state->part_angle_x = parts[30].unk50[0];
        ai_state->part_angle_y = parts[30].unk50[1];
        ai_state->part_angle_z = parts[31].unk50[0];
        ai_state->pitch_speed = parts[31].unk50[1];
    } else {
        pitch = atan2f(player_state.position[1] - entity->model->position[1], entity->distance_to_player);
        pitch_limit = 0.5235988f;
        if (pitch_limit < pitch) {
            pitch = pitch_limit;
        } else if (pitch < -pitch_limit) {
            pitch = -pitch_limit;
        }
        half_pitch = pitch * 0.5f;
        EntityMath_EaseAngleToward(&ai_state->part_angle_x, half_pitch, 10.0f);
        EntityMath_EaseAngleToward(&ai_state->part_angle_z, half_pitch, 10.0f);
        parts[20].unk50[0] = ai_state->part_angle_x;
        parts[21].unk50[0] = ai_state->part_angle_z;
        yaw_to_target = EntityMath_HeadingTo(entity->model->position, entity->target);
        yaw = EntityMath_AngleDifference(entity->model->rotation[1], yaw_to_target);
        yaw_limit = 0.6981317f;
        if (yaw_limit < yaw) {
            yaw = yaw_limit;
        } else if (yaw < -yaw_limit) {
            yaw = -yaw_limit;
        }
        half_yaw = yaw * 0.5f;
        EntityMath_EaseAngleToward(&ai_state->part_angle_y, half_yaw, 10.0f);
        EntityMath_EaseAngleToward(&ai_state->pitch_speed, half_yaw, 10.0f);
        parts[20].unk50[1] = ai_state->part_angle_y;
        parts[21].unk50[1] = ai_state->pitch_speed;
    }
}

/* The think handler: as EntityAi_Think, with WiddaArcher_ThinkCombat for state 2. Before that it sets bit 40
   of model part 29's unk68 during frames 7 to 35 of animation 23 and frames 25 to 49 of animation
   24, and clears it otherwise. */
static void WiddaArcher_Think(Entity *entity, s32 unknown) {
    if (entity->model->level_tables->blend_left == 0 &&
        ((entity->animation_index == 0x17 && entity->model->level_tables->frame >= 7 && entity->model->level_tables->frame < 0x24) ||
         (entity->animation_index == 0x18 && entity->model->level_tables->frame >= 0x19 && entity->model->level_tables->frame < 0x32))) {
        ((EntityModelPart80 *)entity->model->parts)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->parts)[29].unk68 &= ~(0x8000L << 25);
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        WiddaArcher_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, with WiddaArcher_RangedAttack for a ranged attack and
   a 40-degree arc for the dodge; closing in only turns to the player (EntityAi_TurnCommand) for 120 to 179
   frames. On every tenth frame, with Entity.distance_to_player_3d less the radius and 100 under 200 and the entity within 40
   degrees of the player's facing, it may start EntityAi_Dodge (the more health lost, the likelier) when
   Entity.heading_to_player is 40 to 90 degrees off the heading. */
static void WiddaArcher_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.6981317f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    side = EntityAi_PickCombatChoice(entity, 0);
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, WiddaArcher_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
        if (frame_counter % 10U == entity->spawn_index % 10U &&
            rand() % 100 < (entity->params->hit_points - entity->hit_points) * 100 / entity->params->hit_points) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                EntityAi_StartAction(entity, EntityAi_Dodge, 1);
                return;
            }
        }
    }
    if (ai_state->move_choice == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = EntityAi_TurnCommand(entity);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but its shots are aimed at a point 190
   below the player's position, within 10 degrees (EntityShot_LaunchAimed). */
static void WiddaArcher_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, player_state.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, target, 0.17453294f, 0.17453294f);
        if (entity->model->level_tables->frame > 0 && unk3D8->unk03 >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* The think handler of a spawn whose EntitySpawnInfo.spawn_mode is set: as WiddaArcher_Think, but while
   the byte of map_script_flags that EntitySpawnInfo.map_object_id and variant index is not 0 it calls
   WiddaArcher_WalkToMapObject instead of stepping the states. */
static void WiddaArcher_ThinkUseMapObject(Entity *entity, s32 unknown) {
    if (entity->model->level_tables->blend_left == 0 &&
        ((entity->animation_index == 0x17 && entity->model->level_tables->frame >= 7 && entity->model->level_tables->frame < 0x24) ||
         (entity->animation_index == 0x18 && entity->model->level_tables->frame >= 0x19 && entity->model->level_tables->frame < 0x32))) {
        ((EntityModelPart80 *)entity->model->parts)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->parts)[29].unk68 &= ~(0x8000L << 25);
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    if (map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 64] != 0) {
        WiddaArcher_WalkToMapObject(entity, unknown);
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        WiddaArcher_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of WiddaArcher_ThinkUseMapObject, which calls it in place of the state functions: walks to a
   point 100 in z from the map object EntitySpawnInfo.second_map_object_id names, turns to the object there, and
   then starts WiddaArcher_StartMapObjectScript; until then it starts the move action. */
static void WiddaArcher_WalkToMapObject(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;
    MapObject *map_object;

    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
    if (map_object != NULL) {
        stand_point[0] = map_object->model->position.x;
        stand_point[1] = map_object->model->position.y;
        stand_point[2] = map_object->model->position.z + 100.0f;
        stand_point[3] = 0.0f;
        face_point[0] = map_object->model->position.x;
        face_point[1] = map_object->model->position.y;
        face_point[2] = map_object->model->position.z;
        face_point[3] = 0.0f;
    }
    reach = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        Vector_Copy(ai_state->target_point, face_point);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_TurnCommand(entity);
        if (entity->move_command == 0) {
            EntityAi_StartAction(entity, WiddaArcher_StartMapObjectScript, unknown);
            return;
        }
    } else {
        Vector_Copy(ai_state->target_point, stand_point);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* An action (busy bit 0x1000) that WiddaArcher_WalkToMapObject starts: plays animation 17 to its
   end; on frame EntitySpawnInfo.script_anim_frame it starts the script (Script_Start) of the map object
   EntitySpawnInfo.second_map_object_id names, when the object has one and the entity is on map current_map_slot. */
static void WiddaArcher_StartMapObjectScript(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ai_state->turn_rate = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame == entity->spawn_info->script_anim_frame) {
            map_object = MapObject_GetById(entity->group_id, entity->spawn_info->second_map_object_id);
            if (map_object != NULL && map_object->source->use_script != 0 && entity->group_id == current_map_slot) {
                Script_Start(map_object->source->use_script, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}
