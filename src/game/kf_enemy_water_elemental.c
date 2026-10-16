/* WaterElemental: the behaviour of entity type 51 (Entity.params->unk02, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Water Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
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
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
extern PlayerData player_state;
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_DistanceXZ(void *, void *);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);

static void WaterElemental_Think(Entity *entity, s32 unknown);
static void WaterElemental_ThinkCombat(Entity *entity, s32 unknown);
static void WaterElemental_PlayTurnAnim(Entity *entity);
static void WaterElemental_RecoverHealth(Entity *entity);
static s32 WaterElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle);
static void WaterElemental_AlertSameKind(Entity *entity);
static void WaterElemental_OnHit(Entity *entity);
static void WaterElemental_RunAtPlayer(Entity *entity);

/* Set-up of a type 51 entity: clears its work area and installs the type's handlers. */
void WaterElemental_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = WaterElemental_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))WaterElemental_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~0x10000000;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
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

/* The think handler: as EntityAi_Think, with WaterElemental_ThinkCombat for state 2. Work byte unk7B
   is put to 30 while player_state.unk259 is set and counts down. While work word unk68 is 1
   (WaterElemental_AlertSameKind of another entity sets it; states 0 and 1 clear it) it only starts
   WaterElemental_RunAtPlayer. Once the pause in work word unk6C is over and Entity.hit_points is below
   EntityParams.near_distance percent of EntityParams.hit_points it starts WaterElemental_RecoverHealth with a
   chance of unk438 in 100. */
static void WaterElemental_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->mode_timer > 0) {
        unkE0->mode_timer--;
    }
    if (player_state.magic_shot_launched != 0) {
        unkE0->action_flags = 30;
    }
    if (unkE0->action_flags > 0) {
        unkE0->action_flags--;
    }
    if (unkE0->pause_timer == 1) {
        if (entity->busy_flags == 0) {
            EntityAi_StartAction(entity, WaterElemental_RunAtPlayer, 1);
        }
        return;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (unkE0->mode_timer == 0 && (s32)(entity->hit_points * 100.0f / entity->params->hit_points) < entity->params->near_distance
            && rand() % 100 < entity->params->far_distance) {
            EntityAi_StartAction(entity, WaterElemental_RecoverHealth, 1);
            return;
        }
        if ((s32)(entity->flags >> 53) & 1) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                unkE0->pause_timer = 0;
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                unkE0->pause_timer = 0;
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                WaterElemental_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   WaterElemental_ShouldDodgeMagic says so, or on every fifteenth frame with the player nearer than the radius plus
   400 and less than 90 degrees off its heading. Closing in is at the run; it starts
   WaterElemental_PlayTurnAnim now and then when the player is more than 90 degrees off its heading.
   */
static void WaterElemental_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if ((entity->distance_to_player < entity->radius + 400.0f
         && ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < 1.5707964f && frame_counter % 15U == 0
         && rand() % 50 < 80)
        || WaterElemental_ShouldDodgeMagic(entity, 200.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= 0x200;
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        // match: called with the entity alone here
        ((void (*)(Entity *))EntityAi_LowerChoiceWeight)(entity);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
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
        if ((s32)(entity->flags >> 31) & 1) {
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
            side = EntityAi_TurnFromPush(entity, direction);
            if (side > 0) {
                entity->move_command = 0x241;
            } else if (side < 0) {
                entity->move_command = 0x141;
            } else {
                if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > 1.5707964f
                    && frame_counter % 15U == 0 && !(rand() & 1)) {
                    unkE0->step_index = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, WaterElemental_PlayTurnAnim, unknown);
                    return;
                }
                entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x2000): plays animation 12 or, with work byte unk7A below 1, animation 11 to
   its end. WaterElemental_ThinkCombat starts it when the player is more than 90 degrees off the
   heading. */
static void WaterElemental_PlayTurnAnim(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->busy_flags |= 0x2000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->step_index > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that WaterElemental_Think starts when Entity.hit_points is below
   EntityParams.near_distance percent of EntityParams.hit_points: plays animation 18 to its end; on frame
   EntityParams.unk43E it adds EntityParams.unk43A percent of unk0A to Entity.hit_points, once, and sets
   the pause in work word unk6C to unk43C. Entity.hit_points is the entity's health: Entity_ResetToSpawn
   loads it from EntityParams.hit_points, Attack_HitEntity takes the damage of a hit off it, and
   EntityAi_Update starts the death action when it is 0. */
static void WaterElemental_RecoverHealth(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->busy_flags |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        unkE0->action_counter = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->action_counter == 0 && entity->model->level_tables->unk82 == entity->params->unk43E) {
            entity->hit_points += (s32)(entity->params->hit_points * (f32)entity->params->unk43A / 100.0f);
            unkE0->mode_timer = entity->params->unk43C;
            unkE0->action_counter = 1;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Whether to dodge a magic: only while work byte unk7B is 1 to 15 (WaterElemental_Think puts it to 30 when the
   player casts one, player_state.unk259), with the player at least min_distance away (less the radius and 100),
   Entity.heading_to_player within max_angle of the heading and the entity within 30 degrees of the player's facing; then
   EntityParams.dodge_chance / 10 times in 100. */
static s32 WaterElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if ((u8)(entity->unkE0.action_flags - 1) >= 15) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    player_angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (ABS(player_angle) > 0.5235988f) {
        return FALSE;
    }
    return rand() % 100 < entity->params->dodge_chance / 10;
}

/* Helper of WaterElemental_OnHit: sets work word unk68 to 1 in every other live entity with the same
   EntityParams that is within EntityParams.action_frames of this one, has unk68 at 0 and has the player
   within EntitySpawnInfo.wander_radius of its spawn point. */
static void WaterElemental_AlertSameKind(Entity *entity) {
    Entity *other;

    for (other = active_entities_head; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityUnkE0 *unkE0 = &other->unkE0;

            if (unkE0->pause_timer == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position)
                       < (s16)entity->params->action_frames
                && EntityMath_DistanceXZ(other->spawn_info, player_state.position) < other->spawn_info->wander_radius) {
                unkE0->pause_timer = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then WaterElemental_AlertSameKind. */
static void WaterElemental_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    WaterElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that WaterElemental_Think starts while work word unk68 is 1: plays
   animation 3, then each frame guards or dodges the player's swing, starts the attack EntityAi_PickAttack
   picks once EntityAi_PickCombatChoice chooses attacking (unk68 becomes 2), and else gives the command to run
   at the player and calls the move handler itself. Flag bit 53 ends it with unk68 at 2. */
static void WaterElemental_RunAtPlayer(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    entity->busy_flags |= 0x3000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (WaterElemental_ShouldDodgeMagic(entity, 200.0f, 1.0471976f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Guard, 1);
            break;
        }
        if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            unkE0->pause_timer = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        side = EntityAi_PickCombatChoice(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->prev_combat_choice = entity->combat_choice;
            entity->combat_choice = 0;
            if (entity->prev_combat_choice != 0) {
                entity->choice_change_frame = frame_counter;
            }
            EntityAi_LowerChoiceWeight(entity, 0);
            if (entity->combat_choice == 0) {
                unkE0->pause_timer = 2;
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            }
            break;
        }
        entity->target = &player_state;
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command = 0x241;
        } else if (side < 0) {
            entity->move_command = 0x141;
        } else {
            entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        ((void (*)(Entity *))unkE0->move)(entity);
        break;
    }
}
