/* FireGenie: the behaviour of entity type 66 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Fire Genie. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "entity/entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern f32 identity_matrix[16];
float EntityMath_AngleDifference(float, float);
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
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
float EntityMath_WrapAngle(float);
// match: the definitions take the float last, but retail sets it up before the index
int EntityAi_CanUseMeleeFacing(Entity *, f32, s32);
int EntityAi_CanUseRangedFacing(Entity *, f32, s32); // match: as EntityAi_CanUseMeleeFacing
int rand(void);
void EntityAi_LowerAttackWeight(Entity *);
void EntityEvent_Respawn(char *);

static void FireGenie_Animate(Entity *entity);
static void FireGenie_TurnPart3ToPlayer(Entity *entity, s32 mode);
static void FireGenie_OnHit(Entity *entity);
static void FireGenie_Think(Entity *entity, s32 unknown);
static void FireGenie_ThinkDecide(Entity *entity, s32 unknown);
static void FireGenie_RangedAttack0(Entity *entity);
static void FireGenie_RespawnOther(Entity *entity);
static void FireGenie_Death(Entity *entity);
static void FireGenie_Move(Entity *entity);
static s32 FireGenie_CanAttack(Entity *entity);
static s32 FireGenie_PickAttack(Entity *entity);
static s32 FireGenie_ShouldRespawnOther(Entity *entity);
static void FireGenie_RespawnOneOther(Entity *entity);
static void FireGenie_SignalRespawned(Entity *entity);

/* Set-up of a type 66 entity: clears its work area and installs the type's handlers. */
void FireGenie_Init(Entity *entity) {
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
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = FireGenie_Think;
    ai_state->animate = (void (*)(void))FireGenie_Animate;
    ai_state->on_hit = (void (*)(void))FireGenie_OnHit;
    ai_state->move = (void (*)(void))FireGenie_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))FireGenie_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
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

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and
   FireGenie_TurnPart3ToPlayer turns model part 3 (mode 0 while a blend is on, mode 1 otherwise once
   the motion is past frame 0). */
static void FireGenie_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    s32 look_at_target;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
            FireGenie_TurnPart3ToPlayer(entity, 0);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
            if (entity->model->level_tables->frame) {
                FireGenie_TurnPart3ToPlayer(entity, 1);
            }
        }
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        look_at_target = (s32)(entity->flags >> 9) & 1;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of FireGenie_Animate: in mode 1 turns model part 3 by the angle of Entity.heading_to_player off the
   heading, kept between 0 and 40 degrees (work float unk48), worked out through the part's parent
   matrix; the part's three angles are work floats unk4C to unk54. In mode 0 it only puts those back
   on the part. */
static void FireGenie_TurnPart3ToPlayer(Entity *entity, s32 mode) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *bone = &((EntityModelPart80 *)entity->model->parts)[3];
    f32 parent[16];
    f32 rotation[16];
    f32 twist[16];
    f32 angles[4];
    f32 yaw;
    f32 rate;

    if (bone != NULL) {
        if (mode != 0) {
            yaw = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (yaw > 0.6981317f) {
                yaw = 0.6981317f;
            }
            if (yaw < 0.0f) {
                yaw = 0.0f;
            }
            rate = 10.0f;
            EntityMath_EaseAngleToward(&ai_state->part_angle_x, yaw, rate);
            sceVu0CopyMatrix(parent, identity_matrix);
            Bone_MultiplyPathTo(entity->model->parts, bone, parent);
            parent[12] = 0.0f;
            parent[13] = 0.0f;
            parent[14] = 0.0f;
            parent[15] = 1.0f;
            sceVu0RotMatrixX(rotation, identity_matrix, bone->unk50[0]);
            sceVu0RotMatrixZ(rotation, rotation, bone->unk50[2]);
            sceVu0RotMatrixY(rotation, rotation, bone->unk50[1]);
            Matrix_Multiply(rotation, parent, rotation);
            sceVu0RotMatrixY(twist, identity_matrix, ai_state->part_angle_x);
            Matrix_Multiply(rotation, twist, rotation);
            sceVu0TransposeMatrix(parent, parent);
            Matrix_Multiply(rotation, parent, rotation);
            Bone_MatrixToAngles(angles, rotation);
            EntityMath_EaseAngleToward(&ai_state->part_angle_y, angles[0], rate);
            EntityMath_EaseAngleToward(&ai_state->part_angle_z, angles[1], rate);
            EntityMath_EaseAngleToward(&ai_state->pitch_speed, angles[2], rate);
            bone->unk50[0] = ai_state->part_angle_y;
            bone->unk50[1] = ai_state->part_angle_z;
            bone->unk50[2] = ai_state->pitch_speed;
        } else {
            bone->unk50[0] = ai_state->part_angle_y;
            bone->unk50[1] = ai_state->part_angle_z;
            bone->unk50[2] = ai_state->pitch_speed;
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy); with work byte unk7C set that hit also puts -1 in work byte unk7A of
   the entity with spawn id work byte unk7B, and clears unk7C. */
static void FireGenie_OnHit(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u64 flags;
    Entity *other;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    flags = entity->flags;
    if (((flags >> 36) & 3) == 2) {
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
        if (ai_state->action_counter) {
            other = Entity_GetBySpawnId(ai_state->action_flags);
            if (other != NULL) {
                other->ai_state.step_index = -1;
            }
        }
        ai_state->action_counter = 0;
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: no states; every tenth frame updates what the entity knows of the player, and
   while it is not busy calls FireGenie_ThinkDecide. Counts down work word unk68. */
static void FireGenie_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        FireGenie_ThinkDecide(entity, unknown);
    }
    if (ai_state->pause_timer != 0) {
        ai_state->pause_timer--;
    }
}

/* Decides for FireGenie_Think: guards the player's swing; starts FireGenie_RespawnOther when
   FireGenie_ShouldRespawnOther says so; when an attack reaches (FireGenie_CanAttack) starts
   the one FireGenie_PickAttack draws, FireGenie_RangedAttack0 for ranged record 0; else
   starts the move action. */
static void FireGenie_ThinkDecide(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
    } else {
        entity->flags |= ENTITY_FLAG_400;
        entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
        if (FireGenie_ShouldRespawnOther(entity)) {
            EntityAi_StartAction(entity, FireGenie_RespawnOther, unknown);
        } else if (entity->attack_delay_frames == 0 && FireGenie_CanAttack(entity)) {
            if (FireGenie_PickAttack(entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 0) {
                    EntityAi_StartAction(entity, FireGenie_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        }
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, no turning; from frame
   EntityParams.action_frames to unk436 it sets flag bit 9 and launches the record's shots on two frames in
   three. */
static void FireGenie_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= (s16)entity->params->action_frames
            && entity->model->level_tables->frame <= entity->params->near_distance) {
            ENTITY_FLAGS(entity) |= 0x200;
            if (frame_counter % 3U != 0) {
                EntityAi_LaunchShots(entity, unk3D8, (s16)entity->params->action_frames, entity->params->near_distance);
            }
        }
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

/* An action (busy bit 0x1000) that FireGenie_ThinkDecide starts when
   FireGenie_ShouldRespawnOther says so: plays animation 17 to its end. Past frame
   EntityParams.far_distance it calls FireGenie_RespawnOneOther and sets work byte unk7C; past frame
   type_param_0 it calls FireGenie_SignalRespawned and clears unk7C. */
static void FireGenie_RespawnOther(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->params->far_distance < entity->model->level_tables->frame) {
            FireGenie_RespawnOneOther(entity);
            ENTITY_FLAGS(entity) |= 0x200;
            ai_state->action_counter = 1;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->params->type_param_0 < entity->model->level_tables->frame) {
            FireGenie_SignalRespawned(entity);
            ai_state->action_counter = 0;
            entity->action_step++;
        }
        break;
    case 4:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; with work byte unk7C set it first puts -1 in work byte
   unk7A of the entity with spawn id work byte unk7B. */
static void FireGenie_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) &= ~0x200;
        if (ai_state->action_counter != 0) {
            Entity *other = Entity_GetBySpawnId(ai_state->action_flags);

            if (other != NULL) {
                other->ai_state.step_index = -1;
            }
        }
        ai_state->action_counter = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            ENTITY_FLAGS(entity) |= 0x8000000;
            ENTITY_FLAGS(entity) |= 0x4000000;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void FireGenie_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Whether the player is in reach of any of the six attack records (EntityAi_CanUseMeleeFacing for the melee
   ones, EntityAi_CanUseRangedFacing for the ranged), along the heading of the turned part (work float unk48). */
static s32 FireGenie_CanAttack(Entity *entity) {
    f32 angle = EntityMath_WrapAngle(entity->model->rotation[1] + entity->ai_state.part_angle_x);

    if (EntityAi_CanUseMeleeFacing(entity, angle, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, angle, 1)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, angle, 2)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, angle, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, angle, 1)) {
        return TRUE;
    }
    return EntityAi_CanUseRangedFacing(entity, angle, 2) != 0;
}

/* Draws one of the six attacks by weight among those that reach the player, sets flag bits 57 to 59
   to it and returns whether it is a ranged one: EntityAi_PickAttack with the heading of the turned part.
   */
static s32 FireGenie_PickAttack(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;
    f32 angle;

    angle = EntityMath_WrapAngle(entity->model->rotation[1] + entity->ai_state.part_angle_x);
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 0) ? (entity->attack_weights[0] * entity->params->melee_attacks[0].attack_weight) >> 6 : 0;
    chances[0] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 1) ? (entity->attack_weights[1] * entity->params->melee_attacks[1].attack_weight) >> 6 : 0;
    chances[1] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 2) ? (entity->attack_weights[2] * ((EntityParamsUnk240 *)entity->params->melee_attack_third)->attack_weight) >> 6 : 0;
    chances[2] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 0) ? (entity->attack_weights[3] * ENTITY_PARAMS_UNK3D8(entity->params, 0)->unk00) >> 6 : 0;
    chances[3] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 1) ? (entity->attack_weights[4] * ENTITY_PARAMS_UNK3D8(entity->params, 1)->unk00) >> 6 : 0;
    chances[4] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 2) ? (entity->attack_weights[5] * ENTITY_PARAMS_UNK3D8(entity->params, 2)->unk00) >> 6 : 0;
    chances[5] = chance;
    total += chance;
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & 1;
}

/* Helper of FireGenie_ThinkDecide: counts the entities with spawn ids 10 to 9 plus
   EntityParams.type_param_1 that have flag bits 0 and 1 set and flag bits 2-3 below 3 (the count goes to
   work byte unk7A, a bit each to work word unk6A); TRUE when there are fewer than type_param_1 and the
   pause in work word unk68 is over. */
static s32 FireGenie_ShouldRespawnOther(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 count = 0;
    s32 i;

    ai_state->sub_timer = 0;
    for (i = 0; i < entity->params->type_param_1; i++) {
        Entity *other = Entity_GetBySpawnId(i + 10);
        if (other != NULL && (other->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) == 3 && ((s32)(other->flags >> 2) & 3) < 3) {
            ai_state->sub_timer |= 1 << i;
            count++;
        }
    }
    ai_state->step_index = count;
    if (ai_state->step_index < entity->params->type_param_1) {
        if (ai_state->pause_timer < 1) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Helper of FireGenie_RespawnOther: draws one of the entities FireGenie_ShouldRespawnOther
   did not count, respawns it when it has flag bit 0 set (EntityEvent_Respawn, which calls
   Entity_ResetToSpawn), keeps its spawn id in work byte unk7B, and sets the pause in work word
   unk68 to EntityParams.type_param_2. */
static void FireGenie_RespawnOneOther(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 remaining = 1;
    s32 i;
    s32 slots_left = entity->params->type_param_1 - ai_state->step_index;

    for (i = 0; i < entity->params->type_param_1; i++) {
        if (!((ai_state->sub_timer >> i) & 1)) {
            if (rand() % 100 < remaining * 100 / slots_left) {
                s32 other_id = i + 10;
                Entity *other = Entity_GetBySpawnId(other_id);
                if (other != NULL && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST)) {
                    EntityEvent_Respawn((char *)other);
                    ai_state->action_flags = other_id;
                    remaining--;
                }
            }
            if (remaining <= 0) {
                break;
            }
            slots_left--;
            if (slots_left <= 0) {
                break;
            }
        }
    }
    ai_state->pause_timer = entity->params->type_param_2;
}

/* Helper of FireGenie_RespawnOther: puts 1 in work byte unk7A of the entity
   FireGenie_RespawnOneOther respawned (its spawn id is in work byte unk7B). */
static void FireGenie_SignalRespawned(Entity *entity) {
    Entity *other = Entity_GetBySpawnId(entity->ai_state.action_flags);

    if (other != NULL) {
        other->ai_state.step_index = 1;
    }
}
