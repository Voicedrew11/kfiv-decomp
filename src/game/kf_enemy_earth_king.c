/* EarthKing: the behaviour of entity type 45 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Earth King. The game names no
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
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_StepState(Entity *);
void EntityAi_SensePlayer(Entity *, int);
void EntityAi_CheckFarFromSpawn(Entity *);
extern float player_state[];
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHitAt(Entity *, void *, s32, s32, s32, f32);
float EntityMath_WrapAngle(float);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
s32 EntityAi_SendMeleeHit(Entity *, void *, s32, f32);

static void EarthKing_Think(Entity *entity, s32 unknown);
static void EarthKing_ThinkCombat(Entity *entity, s32 unknown);
static void EarthKing_MeleeAttack2(Entity *entity);
static void EarthKing_RangedAttack0(Entity *entity);
static void EarthKing_RangedAttack1(Entity *entity);

/* Set-up of a type 45 entity: clears its work area and installs the type's handlers. */
void EarthKing_Init(Entity *entity) {
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
    unkE0->think = EarthKing_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
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

/* The think handler: as EntityAi_Think, with EarthKing_ThinkCombat for states 1 and 2. */
static void EarthKing_Think(Entity *entity, s32 unknown) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        EarthKing_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of states 1 and 2: as EntityAi_ThinkCombat, but ranged record 0 is
   EarthKing_RangedAttack0, the other ranged records EarthKing_RangedAttack1 and melee record
   2 EarthKing_MeleeAttack2; closing in never runs. */
static void EarthKing_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    EntityAi_StartAction(entity, EarthKing_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EarthKing_RangedAttack1, unknown);
                }
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 2) {
                EntityAi_StartAction(entity, EarthKing_MeleeAttack2, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = player_state;
            entity->action_timer = rand() % 60 + 120;
            unkE0->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            unkE0->move_choice = -1;
            break;
        default:
            unkE0->move_choice = -1;
            break;
        }
    }
    if (unkE0->move_choice == 0) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (unkE0->turn_in_place == 0) {
                unkE0->turn_in_place = ABS(angle) > 0.5235988f;
                if (unkE0->turn_in_place == 0) {
                    goto done;
                }
            }
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                unkE0->turn_in_place = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->move_command = 0x201;
            } else if (turn < 0) {
                entity->move_command = 0x101;
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The melee attack with record 2 (EntityParams.melee_attack_third; busy bit 1): animation 22; the three parts
   are checked along the heading plus EntityParams.far_distance, unk43A and unk43C degrees, and one hit
   ends the checks. */
static void EarthKing_MeleeAttack2(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = (EntityParamsUnk240 *)entity->params->melee_attack_third;
    s32 frame;

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x16, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        } else {
            unkE0->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + entity->params->far_distance * 0.017453292f));
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + entity->params->unk43A * 0.017453292f));
                if (!ENTITY_FLAG_BITS(entity)->bit54) {
                    ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->unk82, EntityMath_WrapAngle(entity->model->rotation[1] + entity->params->unk43C * 0.017453292f));
                }
            }
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

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.action_frames to
   unk436 it sets flag bit 9 and launches the record's shots every other frame. */
static void EarthKing_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        } else {
            unkE0->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->unk82 >= (s16)entity->params->action_frames
            && entity->model->level_tables->unk82 <= entity->params->near_distance) {
            ENTITY_FLAGS(entity) |= 0x200;
            if (frame_counter & 1) {
                EntityAi_LaunchShots(entity, unk3D8, (s16)entity->params->action_frames, entity->params->near_distance);
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

/* The ranged attack with record 1 (busy bit 1): animation 24, as EntityAi_RangedAttack; on frames
   38 to 40 it also checks part 0 of melee record 0 for a hit along the heading (EntityAi_SendMeleeHit). */
static void EarthKing_RangedAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];

    entity->busy_flags |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x18, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->turn_rate, entity->heading_to_player);
        } else {
            unkE0->turn_rate = 0.0f;
        }
        EntityAi_LaunchShots(entity, unk3D8,
                      (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->unk82);
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            if (entity->model->level_tables->unk82 >= 0x26 && entity->model->level_tables->unk82 <= 0x28) {
                ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_SendMeleeHit(entity, unk240, 0, entity->model->rotation[1]);
            }
        }
        if (entity->model->level_tables->unk82 > 0 && unk3D8->unk03 >= entity->model->level_tables->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
