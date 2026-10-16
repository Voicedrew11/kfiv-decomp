/* WiddaQueen: the behaviour of entity type 46 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Queen. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_EaseAngleToward(float *, float, float);
extern u32 D_00446ED4;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
void EntityAi_Guard(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_CheckMeleeHitAt(Entity *, void *, s32, s32, s32, f32);
float EntityMath_WrapAngle(float);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
extern PlayerData player_state;
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
// match: the definitions take the float last, but retail sets it up before the index
int EntityAi_CanUseMeleeFacing(Entity *, f32, s32);
int EntityAi_CanUseRangedFacing(Entity *, f32, s32); // match: as EntityAi_CanUseMeleeFacing
int rand(void);
void EntityAi_LowerAttackWeight(Entity *);
void EntityEvent_OnDeath(Entity *);

static void WiddaQueen_Animate(Entity *entity);
static void WiddaQueen_TurnPart9ToPlayer(Entity *entity, s32 mode);
static void WiddaQueen_Think(Entity *entity, s32 unknown);
static void WiddaQueen_ThinkDecide(Entity *entity, s32 unknown);
static void WiddaQueen_MeleeAttack0(Entity *entity);
static void WiddaQueen_MeleeAttack1(Entity *entity);
static void WiddaQueen_RangedAttack0(Entity *entity);
static void WiddaQueen_RangedAttack1(Entity *entity);
static void WiddaQueen_Move(Entity *entity);
static s32 WiddaQueen_CanAttack(Entity *entity);
static s32 WiddaQueen_PickAttack(Entity *entity);
static void WiddaQueen_Death(Entity *entity);

/* Set-up of a type 46 entity: clears its work area and installs the type's handlers. */
void WiddaQueen_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = WiddaQueen_Think;
    unkE0->animate = (void (*)(void))WiddaQueen_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))WiddaQueen_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))WiddaQueen_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->look_part_angles, unk138 + 0x50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and
   WiddaQueen_TurnPart9ToPlayer turns model part 9. */
static void WiddaQueen_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->level_tables->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & 0x12) ? unkE0->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        WiddaQueen_TurnPart9ToPlayer(entity, unkE0->step_index);
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of WiddaQueen_Animate: in mode 1 turns model part 9 towards the player, at most 90
   degrees to either side, at the rate EntityParams.near_distance; in mode 2 holds it; else puts the angle
   (work float unk48) back to 0. The mode is work byte unk7A. */
static void WiddaQueen_TurnPart9ToPlayer(Entity *entity, s32 mode) {
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    f32 rate;
    f32 angle;

    unkE0 = &entity->unkE0;
    rate = (f32)entity->params->near_distance;
    part = &((EntityModelPart80 *)entity->model->parts)[9];
    if (entity->ailment_3_frames != 0) {
        rate += rate;
    }
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (1.5707964f < angle) {
        angle = 1.5707964f;
    }
    if (angle < -1.5707964f) {
        angle = -1.5707964f;
    }
    if (mode == 1) {
        EntityMath_EaseAngleToward(&unkE0->part_angle_x, angle, rate);
        part->unk50[1] = unkE0->part_angle_x;
    } else if (mode == 2) {
        part->unk50[1] = unkE0->part_angle_x;
    } else {
        unkE0->part_angle_x = 0.0f;
    }
}

/* The think handler: no states; every tenth frame updates what the entity knows of the player, and
   while it is not busy calls WiddaQueen_ThinkDecide. */
static void WiddaQueen_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        WiddaQueen_ThinkDecide(entity, unknown);
    }
}

/* Decides for WiddaQueen_Think: guards the player's swing; when an attack reaches
   (WiddaQueen_CanAttack) starts the one WiddaQueen_PickAttack draws; else starts the move
   action. */
static void WiddaQueen_ThinkDecide(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 3.1415927f)) {
        // match: called as returning int; only then is the 2 below loaded into $3
        ((int (*)(char *, void (*)(void), int))EntityAi_StartAction)((char *)entity, (void (*)(void))EntityAi_Guard, 1);
        unkE0->step_index = 2;
        return;
    }
    unkE0->step_index = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->attack_delay_frames == 0 && !(s32)(entity->flags >> 63) && WiddaQueen_CanAttack(entity)) {
        if (WiddaQueen_PickAttack(entity)) {
            if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, WiddaQueen_RangedAttack0, unknown);
            } else {
                EntityAi_StartAction(entity, WiddaQueen_RangedAttack1, unknown);
            }
        } else {
            if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, WiddaQueen_MeleeAttack0, unknown);
            } else {
                EntityAi_StartAction(entity, WiddaQueen_MeleeAttack1, unknown);
            }
        }
        return;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The melee attack with record 0 (busy bit 1): animation 20, its hits checked along the heading of
   the turned part (work float unk48). */
static void WiddaQueen_MeleeAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    s32 frame;

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            unkE0->step_index = 1;
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->unk82) {
            unkE0->step_index = 1;
        } else {
            unkE0->step_index = 2;
        }
        frame = (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + unkE0->part_angle_x));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + unkE0->part_angle_x));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + unkE0->part_angle_x));
        }
        if (entity->model->level_tables->unk82 > 0 && unk240->hit_end_frame >= entity->model->level_tables->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The melee attack with record 1 (busy bit 1): animation 21 from frame 10; parts 0 and 1 are
   checked 45 degrees to either side; it ends at frame 61. */
static void WiddaQueen_MeleeAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    s32 frame;

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x15, 10, (s16)entity->params->action_frames, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->step_index = 0;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        frame = (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->unk82, entity->model->rotation[1] - 0.7853982f);
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->unk82, entity->model->rotation[1] + 0.7853982f);
            }
        }
        if (entity->model->level_tables->unk82 > 0 && unk240->hit_end_frame >= entity->model->level_tables->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (entity->model->level_tables->unk82 >= 0x3D) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, its shots aimed at the player
   (EntityShot_LaunchAimed); calls EntityEvent_StartEffectAtPoint with 0x373 on frame 22. */
static void WiddaQueen_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0];
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            unkE0->step_index = 1;
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->unk82) {
            unkE0->step_index = 1;
        } else {
            unkE0->step_index = 2;
        }
        if (entity->model->level_tables->unk82 == 0x16 && (entity->ailment_4_frames == 0 || unk3D8->unk0C != 0)) {
            EntityEvent_StartEffectAtPoint((char **)entity, 0x373, 4, (int)entity->model->rotation);
            EntityEvent_StartEffectAtPoint((char **)entity, 0x373, 8, (int)entity->model->rotation);
        }
        elapsed = (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, player_state.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, (EntityParamsUnk3D8 *)unk3D8, elapsed, entity->model->level_tables->unk82, target, 1.5707964f, 1.5707964f);
        if (entity->model->level_tables->unk82 > 0 && unk3D8->unk03 >= entity->model->level_tables->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 1 (busy bit 1): animation 24; from frame EntityParams.far_distance to
   unk43A launches the record's shots on two frames in three. */
static void WiddaQueen_RangedAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x18, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            unkE0->step_index = 1;
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->unk82) {
            unkE0->step_index = 1;
        } else {
            unkE0->step_index = 2;
        }
        if (entity->model->level_tables->unk82 >= entity->params->far_distance && entity->model->level_tables->unk82 <= entity->params->unk43A) {
            if (D_00446ED4 % 3U != 0) {
                EntityAi_LaunchShots(entity, unk3D8, entity->params->far_distance, entity->params->unk43A);
            }
        }
        if (entity->model->level_tables->unk82 > 0 && unk3D8->unk03 >= entity->model->level_tables->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void WiddaQueen_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Whether the player is in reach of any of the six attack records (EntityAi_CanUseMeleeFacing for the melee
   ones, EntityAi_CanUseRangedFacing for the ranged). */
static s32 WiddaQueen_CanAttack(Entity *entity) {
    f32 heading = EntityMath_WrapAngle(entity->model->rotation[1] + entity->unkE0.part_angle_x);

    if (EntityAi_CanUseMeleeFacing(entity, heading, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, entity->model->rotation[1], 1)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, heading, 2)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, heading, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, heading, 1)) {
        return TRUE;
    }
    return EntityAi_CanUseRangedFacing(entity, heading, 2) != 0;
}

/* Draws one of the six attacks by weight among those that reach the player, sets flag bits 57 to 59
   to it and returns whether it is a ranged one: EntityAi_PickAttack with the heading of the turned part.
   */
static s32 WiddaQueen_PickAttack(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;
    f32 angle;

    angle = EntityMath_WrapAngle(entity->model->rotation[1] + entity->unkE0.part_angle_x);
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 0) ? (entity->attack_weights[0] * entity->params->melee_attacks[0].attack_weight) >> 6 : 0;
    chances[0] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, entity->model->rotation[1], 1) ? (entity->attack_weights[1] * entity->params->melee_attacks[1].attack_weight) >> 6 : 0;
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

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings. */
static void WiddaQueen_Death(Entity *entity) {
    entity->busy_flags = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags &= ~0x10000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit26 = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}
