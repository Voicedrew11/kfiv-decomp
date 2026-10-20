/* Reaper: the behaviour of entity type 2 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Reaper. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void Reaper_Think(Entity *entity, s32 run_now);
static void Reaper_ThinkCombat(Entity *entity, s32 run_now);
static void Reaper_Move(Entity *entity);
static void Reaper_Dormant(Entity *entity);
static void Reaper_RangedAttack(Entity *entity);

/* Set-up of a type 2 entity: clears its work area and installs the type's handlers. */
void Reaper_Init(Entity *entity) {
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
    ai_state->think = Reaper_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = Reaper_Move;
    ai_state->dormant = Reaper_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: every tenth frame updates what the entity knows of the player; while it is not
   busy it steps Entity.state and in state 2 calls Reaper_ThinkCombat. States 0 and 1 do
   nothing, and flag bit 53 is not looked at. Always sets flag bit 9. */
static void Reaper_Think(Entity *entity, s32 run_now) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case ENTITY_STATE_UNAWARE:
        case ENTITY_STATE_SEARCHING:
            break;
        case ENTITY_STATE_NOTICED:
            Reaper_ThinkCombat(entity, run_now);
            break;
        }
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
}

/* The think function of state 2: guards the player's swing; starts EntityAi_MeleeAttack when
   EntityAi_CanMelee finds a melee record in reach (EntityAi_PickMeleeAttack picks it) and the pause Entity.attack_delay_frames
   is over; else sets flag bit 32 and, each time the timer runs out with EntityAi_PickCombatChoice choosing the
   attack and EntityAi_CanShoot finding a ranged record in reach (EntityAi_PickRangedAttack picks it), starts
   Reaper_RangedAttack; else starts the move action. It gives no movement command. */
static void Reaper_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_CanMelee(entity) && entity->attack_delay_frames == 0) {
        EntityAi_PickMeleeAttack(entity);
        EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
    } else {
        ENTITY_FLAG_BITS(entity)->player_in_reach = TRUE;
        side = EntityAi_PickCombatChoice(entity, 0);
        if (entity->action_timer == 0) {
            entity->prev_combat_choice = entity->combat_choice;
            entity->combat_choice = side;
            if (entity->combat_choice != entity->prev_combat_choice) {
                entity->choice_change_frame = frame_counter;
            }
            EntityAi_LowerChoiceWeight(entity, side);
            if (entity->combat_choice == COMBAT_CHOICE_ATTACK) {
                if (EntityAi_CanShoot(entity)) {
                    EntityAi_PickRangedAttack(entity);
                    EntityAi_StartAction(entity, Reaper_RangedAttack, run_now);
                    return;
                }
            } else {
                entity->action_timer = rand() % 60 + 0x3C;
            }
        }
        EntityAi_StartAction(entity, ai_state->move, run_now);
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void Reaper_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* The dormant action (busy bit 0x20): holds animation 1 with flag bit 18 set until the player is
   within EntitySpawnInfo.wake_distance or flag bit 43 is set, then goes to state 2 and thinks; unlike
   EntityAi_Dormant it plays no animation 2. */
static void Reaper_Dormant(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but the shots are launched with
   EntityShot_Launch along the reverse of Entity.heading_to_player. */
static void Reaper_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 attack_index = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
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
        direction[0] = direction[2] = direction[3] = 0.0f;
        direction[1] = EntityMath_WrapAngle(entity->heading_to_player + PI);
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
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
