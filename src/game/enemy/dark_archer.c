/* DarkArcher: the behaviour of entity type 55 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Archer. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "map/map.h"
#include "effect/effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
void EntityAi_Move(Entity *);
// match: read as pointers, so that the int store to target is not taken to alias the table
extern void *entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern PlayerData player_state;
extern f32 identity_matrix[16];
double atan2(double, double);
float EntityMath_WrapAngle(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
s32 Bone_MultiplyPathTo(void *, void *, void *);
void Bone_MatrixToAngles(void *, void *);
void Matrix_Multiply(void *, void *, void *);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
int EntityAi_TurnCommand(Entity *);
int EntityAi_StartAttack(Entity *, void (*)(Entity *entity), int, int, int);
int EntityAi_CanUseRanged(Entity *, int);
void EntityAi_SensePlayer(void *, int);
int Motion_Start(void *, int, int, u32, int);
// match: the size is unknown; a complete small type would be read through $gp
extern u8 dark_succubus_fight_started[];
extern MapSlot map_slots[];
extern EntitySpawnInfo *map_spawn_infos[2];
void Game_CountSpawnDeath(u32, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SpawnBoneEffect(Entity *, s32, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
extern const f32 dark_archer_no_floor_height[];
f32 EntityCollision_DistanceToPropFloor(void *, void *);
f32 EntityMath_Approach(f32, f32, f32);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);

static void DarkArcher_Animate(Entity *entity);
static void DarkArcher_PitchPart2ToPlayer(Entity *entity, s32 mode);
static void DarkArcher_OnHit(Entity *entity);
static void DarkArcher_ThinkShootOrTurn(Entity *entity, s32 unknown);
static void DarkArcher_MoveTurnOnly(Entity *entity);
static void DarkArcher_DormantFadeIn(Entity *entity);
static void DarkArcher_DeathWithoutReward(Entity *entity);
static void DarkArcher_Think(Entity *entity, s32 unknown);
static void DarkArcher_ThinkCombat(Entity *entity, s32 unknown);
static void DarkArcher_RangedAttack(Entity *entity);
static void DarkArcher_MoveAlongArc(Entity *entity);

/* Set-up of a type 55 entity: clears its work area and installs the type's handlers. */
void DarkArcher_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->ai_state;

        work_area[i] = 0;
    }
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    *(s32 *)&entity->target = 0; // match: stored as an int, so it can move ahead of the params load
    EntityAi_SetUpdate((char *)entity, (s32)entity_type_handlers[entity->params->entity_type][1]);
    if (entity->spawn_info->spawn_mode != 0) {
        ai_state->think = DarkArcher_ThinkShootOrTurn;
        ai_state->animate = DarkArcher_Animate;
        ai_state->on_hit = DarkArcher_OnHit;
        ai_state->dormant = DarkArcher_DormantFadeIn;
        ai_state->move = DarkArcher_MoveTurnOnly;
        ai_state->death = EntityAi_Death;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        ai_state->think = DarkArcher_Think;
        ai_state->animate = DarkArcher_Animate;
        ai_state->on_hit = DarkArcher_OnHit;
        ai_state->death = EntityAi_Death;
        ai_state->dormant = EntityAi_Dormant;
        ai_state->move = EntityAi_Move;
        if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
            EntityAi_StartAction(entity, ai_state->dormant, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 0);
        }
        entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
        entity->model->alpha = entity->model_alpha;
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler (installed in both modes): as EntityAi_Animate, and
   DarkArcher_PitchPart2ToPlayer turns model part 2, with flag bit 10 as its mode. */
static void DarkArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityParams *params;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        DarkArcher_PitchPart2ToPlayer(entity, (s32)((s64)(entity->flags << 22) >> 32) & ENTITY_FLAG_ACTIVE_LIST);
        params = entity->params;
        limits[0] = DEG_TO_RAD(-params->pitch_limit_deg);
        limits[1] = DEG_TO_RAD(params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-params->yaw_limit_deg);
        if (ENTITY_FLAG_BITS(entity)->look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, -1, NULL, 8.0f);
        }
    }
}

/* Helper of DarkArcher_Animate: with mode above 0 tilts model part 2 (EntityModel.parts[2]) by
   the pitch from the entity to the player, at most 60 degrees up or down, easing the part's three
   angles in work floats unk48 to unk50; with mode 0 eases them back to the part's own angles; else
   copies the part's angles into them. */
static void DarkArcher_PitchPart2ToPlayer(Entity *entity, s32 mode) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelUnkF8 *bone = &entity->model->parts[2];
    f32 parent[16];
    f32 rotation[16];
    f32 tilt[16];
    f32 angles[4];
    f32 pitch;
    f32 rate;

    if (bone != NULL) {
        pitch = EntityMath_WrapAngle(atan2(player_state.position[1] - entity->model->position[1], entity->distance_to_player));
        if (pitch > DEG_TO_RAD(60)) {
            pitch = DEG_TO_RAD(60);
        } else if (pitch < -DEG_TO_RAD(60)) {
            pitch = -DEG_TO_RAD(60);
        }
        if (mode > 0) {
            rate = 10.0f;
            sceVu0CopyMatrix(parent, identity_matrix);
            Bone_MultiplyPathTo(entity->model->parts, bone, parent);
            parent[12] = 0.0f;
            parent[13] = 0.0f;
            parent[14] = 0.0f;
            parent[15] = 1.0f;
            sceVu0RotMatrixX(rotation, identity_matrix, bone->part_angles[0]);
            sceVu0RotMatrixZ(rotation, rotation, bone->part_angles[2]);
            sceVu0RotMatrixY(rotation, rotation, bone->part_angles[1]);
            Matrix_Multiply(rotation, parent, rotation);
            sceVu0RotMatrixX(tilt, identity_matrix, pitch);
            Matrix_Multiply(rotation, tilt, rotation);
            sceVu0TransposeMatrix(parent, parent);
            Matrix_Multiply(rotation, parent, rotation);
            Bone_MatrixToAngles(angles, rotation);
            EntityMath_EaseAngleToward(&ai_state->part_angle_x, angles[0], rate);
            EntityMath_EaseAngleToward(&ai_state->part_angle_y, angles[1], rate);
            EntityMath_EaseAngleToward(&ai_state->part_angle_z, angles[2], rate);
            bone->part_angles[0] = ai_state->part_angle_x;
            bone->part_angles[1] = ai_state->part_angle_y;
            bone->part_angles[2] = ai_state->part_angle_z;
        } else if (mode == 0) {
            rate = 10.0f;
            EntityMath_EaseAngleToward(&ai_state->part_angle_x, bone->part_angles[0], rate);
            EntityMath_EaseAngleToward(&ai_state->part_angle_y, bone->part_angles[1], rate);
            EntityMath_EaseAngleToward(&ai_state->part_angle_z, bone->part_angles[2], rate);
            bone->part_angles[0] = ai_state->part_angle_x;
            bone->part_angles[1] = ai_state->part_angle_y;
            bone->part_angles[2] = ai_state->part_angle_z;
        } else {
            ai_state->part_angle_x = bone->part_angles[0];
            ai_state->part_angle_y = bone->part_angles[1];
            ai_state->part_angle_z = bone->part_angles[2];
        }
    }
}

/* The on_hit handler (installed in both modes): as EntityAi_OnHit, and it clears flag bit 28. */
static void DarkArcher_OnHit(Entity *entity) {
    u64 flags;
    u64 hit_strength;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    flags = entity->flags;
    hit_strength = ENTITY_FLAG_BITS(entity)->hit_strength;
    if (hit_strength == 1) {
        if (ENTITY_FLAG_BITS(entity)->attack_window) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler of a spawn whose EntitySpawnInfo.spawn_mode is set: no states. While the entity is
   not busy it starts DarkArcher_RangedAttack with ranged record 2 when flag bit 63 is clear, the
   pause Entity.attack_delay_frames is over and EntityAi_CanUseRanged finds the player in reach of that record; else it
   starts the move action with the command of EntityAi_TurnCommand, the player as target. Keeps flag bit 10
   clear and bits 11 to 13 at 0. */
static void DarkArcher_ThinkShootOrTurn(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    if (entity->busy_flags == 0) {
        u64 flags = entity->flags | ENTITY_FLAG_LOOK_AT_TARGET;
        u32 busy;

        entity->flags = flags;
        busy = (u32)(flags >> 63);
        if (busy == 0 && entity->attack_delay_frames == 0 && EntityAi_CanUseRanged(entity, 2) != 0) {
            EntityAi_StartAttack(entity, DarkArcher_RangedAttack, unknown, 1, 2);
        } else {
            entity->target = &player_state;
            entity->move_command = EntityAi_TurnCommand(entity);
            EntityAi_StartAction(entity, ai_state->move, unknown);
        }
    }
    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
}

/* The move handler of a spawn whose EntitySpawnInfo.spawn_mode is set: turns as the command says, as
   EntityAi_Move does, moves nowhere and keeps animation 0. */
static void DarkArcher_MoveTurnOnly(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u16 command = entity->move_command;
    f32 turn;
    f32 limit;

    if (command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        limit = -entity->params->turn_limit;
        if (ai_state->turn_rate < limit) {
            ai_state->turn_rate = limit;
        }
    } else if (command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        limit = entity->params->turn_limit;
        if (limit < ai_state->turn_rate) {
            ai_state->turn_rate = limit;
        }
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (command & MOVE_COMMAND_TURN_FAST) {
        turn *= 2.0f;
    }
    entity->turn_delta_yaw += turn;
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* The dormant action of a spawn whose EntitySpawnInfo.spawn_mode is set (busy bit 0x20): animation 0,
   unseen (Entity.model_alpha 0) with flag bits 18, 20 and 21 set until dark_succubus_fight_started[0] is not 0, then fades
   the model in over 20 frames (counted in work byte unk7B), clears the three bits and thinks. */
static void DarkArcher_DormantFadeIn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 0, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_200000;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags |= ENTITY_FLAG_40000;
        entity->model_alpha = 0;
        ai_state->action_flags = 0;
        entity->action_step++;
    case 1:
        if (dark_succubus_fight_started[0] != 0) {
            entity->action_step++;
        }
        break;
    case 2:
        entity->model_alpha = (ai_state->action_flags << 7) / 20;
        ai_state->action_flags++;
        if (ai_state->action_flags > 20) {
            entity->flags &= ~ENTITY_FLAG_200000;
            entity->flags &= ~ENTITY_FLAG_100000;
            entity->flags &= ~ENTITY_FLAG_40000;
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
    entity->model->alpha = entity->model_alpha;
}

/* A death action (busy bit 0x40) that nothing refers to: plays animation 0, and once it has blended in counts
   the death for the spawn (Game_CountSpawnDeath), frees the entity's effects, fades it out over 20 frames and removes
   it. Unlike EntityAi_Death it plays no death animation and does not call EntityEvent_OnDeath (no item, gold,
   experience or script). */
static void DarkArcher_DeathWithoutReward(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_40000;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags |= ENTITY_FLAG_200000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            ENTITY_FLAG_BITS(entity)->animates = 0;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
            entity->action_timer = 20;
            Game_CountSpawnDeath(map_slots[entity->group_id].tag, entity->spawn_info - map_spawn_infos[entity->group_id]);
            Effect_FreeForOwner(entity);
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}

/* The think handler: as EntityAi_Think, with DarkArcher_ThinkCombat for state 2. Sets work byte
   unk7A to whether EntityParams.action_frames is not 0 and flag bit 44 or 50 is set. */
static void DarkArcher_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                DarkArcher_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if ((s16)entity->params->action_frames != 0 && (entity->flags & (ENTITY_FLAG_ON_FLOOR | ENTITY_FLAG_ON_TOP))) {
        ai_state->step_index = 1;
    } else {
        ai_state->step_index = 0;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the ranged attack is
   DarkArcher_RangedAttack (with flag bit 10 set), and instead of closing in it starts
   DarkArcher_MoveAlongArc when work byte unk7A is set, the player is nearer than
   EntityParams.near_distance and player_state.unk14 is within 40 degrees of Entity.heading_to_player. */
static void DarkArcher_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
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
    ENTITY_FLAG_BITS(entity)->attacking = 0;
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
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
                ENTITY_FLAG_BITS(entity)->attacking = 1;
                EntityAi_StartAction(entity, DarkArcher_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            angle = EntityMath_AngleDifference(player_state.heading, entity->heading_to_player);
            if (ai_state->step_index != 0 && entity->distance_to_player < (f32)entity->params->near_distance && ABS(angle) < DEG_TO_RAD(40)) {
                EntityAi_StartAction(entity, DarkArcher_MoveAlongArc, unknown);
                return;
            }
            entity->action_timer = rand() % 60 + 0x78;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (ai_state->move_choice == 0) {
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it keeps flag bit 10 set and starts
   effect 0x379 at model point 8 (EntityAi_SpawnBoneEffect) on frames 21 to 31 with record 0, on frames 36 to
   55 with the others. */
static void DarkArcher_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
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
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        ENTITY_FLAG_BITS(entity)->attacking = 1;
        if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
            if (entity->model->level_tables->frame >= 0x15 && entity->model->level_tables->frame < 0x20) {
                EntityAi_SpawnBoneEffect(entity, 0x379, 8);
            }
        } else {
            if (entity->model->level_tables->frame >= 0x24 && entity->model->level_tables->frame < 0x38) {
                EntityAi_SpawnBoneEffect(entity, 0x379, 8);
            }
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->frame);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that DarkArcher_ThinkCombat starts and that moves the entity itself
   in an arc: plays animation 8 to frame 17, then animation 4 while it moves the entity forwards
   along the heading it had (work float unk54 holds that heading plus 180 degrees, and the speed is
   put on the other end of the axis, which gives the same direction as the forward command), the
   speeds eased in work floats unk58 (y) and unk5C. The speed in y goes to minus EntityParams.type_param_1
   (up) until EntityCollision_DistanceToPropFloor, the distance to the floor below, reads EntityParams.type_param_2 or more,
   then to plus type_param_1 until flag bit 44 or 50 is set; then it plays animation 9 to its end. */
static void DarkArcher_MoveAlongArc(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 height;
    s32 run = FALSE;

    entity->busy_flags |= ENTITY_BUSY_1000;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 1);
        ai_state->heading_speed = 0.0f;
        ai_state->keep_min_distance = 0.0f;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= 0x11) {
            ai_state->pitch_speed = EntityMath_WrapAngle(entity->model->rotation[1] + PI);
            ai_state->heading_speed = -entity->params->far_distance;
            ai_state->keep_min_distance = 0.0f;
            ai_state->turn_rate = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        Motion_Start(entity, 4, 0, 0, 0);
        ai_state->keep_min_distance = EntityMath_Approach(ai_state->keep_min_distance, entity->params->max_speed, entity->params->speed_step_per_frame);
        ai_state->heading_speed = EntityMath_Approach(ai_state->heading_speed, -entity->params->type_param_1, -entity->params->type_param_0);
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        if (height == dark_archer_no_floor_height[0] || height >= entity->params->type_param_2) {
            entity->action_step++;
        }
        break;
    case 4:
        ai_state->keep_min_distance = EntityMath_Approach(ai_state->keep_min_distance, entity->params->max_speed, entity->params->speed_step_per_frame);
        ai_state->heading_speed = EntityMath_Approach(ai_state->heading_speed, entity->params->type_param_1, entity->params->type_param_0);
        if (ENTITY_FLAGS(entity) & (0x8200L << 35)) {
            Motion_Start(entity, 9, 0, 0, 1);
            ai_state->heading_speed = 0.0f;
            ai_state->keep_min_distance = 0.0f;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step++;
        }
        break;
    case 5:
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
    ENTITY_SCRATCH_VECTOR[1] = ai_state->heading_speed;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->keep_min_distance;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, identity_matrix, ai_state->pitch_speed);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: never taken, but until the compiler works that out the branch separates the
    // scratchpad stores above from the loads below, so their addresses are not kept in registers
    if (run) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
    entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
    entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
}
