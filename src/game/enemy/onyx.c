/* Onyx: the behaviour of entity type 74 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Onyx. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "math/vu0.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "model/motion.h"
#include "entity/entity_math.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

/* EntityAiState.move_choice 1 (0 is MOVE_CHOICE_CLOSE_IN): keeps 200 to 800 from the player */
#define ONYX_MOVE_CHOICE_KEEP_RANGE 1

/* ai_state->step_index: the attack group, which sets the parts that take part (Onyx_SwitchAttack) */
#define ONYX_ATTACK_RANGED          0       /* ranged record 0: parts 22 to 31 */
#define ONYX_ATTACK_MELEE_RECORD_1  1       /* melee record 1: parts 11 and 12 */
#define ONYX_ATTACK_MELEE_RECORD_2  2       /* melee record 2: parts 13 to 21 */
#define ONYX_ATTACK_NONE            3       /* no attack group yet: parts 8 to 10 */
/* Onyx_SwitchAttack: Entity.action_step, past step 2 */
#define ONYX_SWITCH_STEP_BLEND_IN   3       /* waits for animation 18 to blend in */
#define ONYX_SWITCH_STEP_END        4       /* ends once animation 18 ends, then sets the reload timer */

static void Onyx_Think(Entity *entity, s32 run_now);
static void Onyx_ThinkCombat(Entity *entity, s32 run_now);
static void Onyx_RangedAttack0(Entity *entity);
static void Onyx_SwitchAttack(Entity *entity);
static s32 Onyx_PickAttack(Entity *entity);
static s32 Onyx_GetKeepRangeCommand(Entity *entity, s32 turn, f32 near_range, f32 far_range);

/* Set-up of a type 74 entity: clears its work area and installs the type's handlers. */
void Onyx_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    EntityModelPart80 *part;
    s32 i;
    s32 part_index;
    s32 first_part;
    s32 part_count;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Onyx_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
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
    ai_state->pause_timer = entity->params->ranged_attacks[0].record.ranged_weight;
    ai_state->sub_timer = entity->params->melee_attacks[1].attack_weight;
    ai_state->mode_timer = (u8)entity->params->melee_attack_third[0];
    ai_state->step_index = ONYX_ATTACK_NONE;
    for (part_index = 0; part_index < 25; part_index++) {
        part = &((EntityModelPart80 *)entity->model->parts)[part_index + 8];
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = TRUE;
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
        ai_state->pause_timer = 0;
        ai_state->sub_timer = 0;
        ai_state->mode_timer = 0;
    }
    switch (ai_state->step_index) {
    case ONYX_ATTACK_RANGED:
        first_part = 0x16;
        part_count = 10;
        ai_state->pause_timer = entity->params->ranged_attacks[0].record.ranged_weight;
        break;
    case ONYX_ATTACK_MELEE_RECORD_1:
        first_part = 0xB;
        part_count = 2;
        ai_state->sub_timer = entity->params->melee_attacks[1].attack_weight;
        break;
    case ONYX_ATTACK_MELEE_RECORD_2:
        first_part = 0xD;
        part_count = 9;
        ai_state->mode_timer = (u8)entity->params->melee_attack_third[0];
        break;
    default:
        first_part = 8;
        part_count = 3;
        ai_state->pause_timer = entity->params->ranged_attacks[0].record.ranged_weight;
        ai_state->sub_timer = entity->params->melee_attacks[1].attack_weight;
        ai_state->mode_timer = (u8)entity->params->melee_attack_third[0];
        break;
    }
    for (part_index = 0; part_index < part_count; part_index++) {
        part = &((EntityModelPart80 *)entity->model->parts)[first_part + part_index];
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = FALSE;
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = FALSE;
    }
    ai_state->reload_timer = entity->params->action_frames;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with Onyx_ThinkCombat for state 2. Counts down work
   word unk6E; once it is 0 (and Onyx_SwitchAttack is not running) it puts the weights of
   ranged record 0 and melee records 1 and 2 back in work words unk68, unk6A and unk6C. */
static void Onyx_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->reload_timer > 0) {
        ai_state->reload_timer--;
    }
    if (!(entity->busy_flags & ENTITY_BUSY_1000) && ai_state->reload_timer == 0) {
        ai_state->pause_timer = entity->params->ranged_attacks[0].record.ranged_weight;
        ai_state->sub_timer = entity->params->melee_attacks[1].attack_weight;
        ai_state->mode_timer = (u8)entity->params->melee_attack_third[0];
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
        Onyx_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the motion never moves the entity and
   Onyx_PickAttack draws the attack. When the draw is not the attack work byte unk7A holds,
   it stores it and starts Onyx_SwitchAttack first; else it starts
   Onyx_RangedAttack0 or EntityAi_MeleeAttack; a draw of melee record 0 makes it close in
   for 30 to 89 frames instead. Closing in with unk7A at 0 keeps a distance
   (Onyx_GetKeepRangeCommand); it never runs. */
static void Onyx_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
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
            ai_state->action_flags = Onyx_PickAttack(entity);
            if (ai_state->action_flags == 0 && ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 0x1E;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
                entity->combat_choice = COMBAT_CHOICE_CLOSE_IN;
                return;
            }
            if (ai_state->action_flags != 0) {
                if (ENTITY_FLAG_BITS(entity)->attack_index == ai_state->step_index) {
                    EntityAi_StartAction(entity, Onyx_RangedAttack0, run_now);
                } else {
                    ai_state->step_index = ENTITY_FLAG_BITS(entity)->attack_index;
                    EntityAi_StartAction(entity, Onyx_SwitchAttack, run_now);
                }
            } else {
                if (ENTITY_FLAG_BITS(entity)->attack_index == ai_state->step_index) {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
                } else {
                    ai_state->step_index = ENTITY_FLAG_BITS(entity)->attack_index;
                    EntityAi_StartAction(entity, Onyx_SwitchAttack, run_now);
                }
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            if (ai_state->step_index == ONYX_ATTACK_RANGED) {
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->move_choice = ONYX_MOVE_CHOICE_KEEP_RANGE;
            } else {
                entity->action_timer = rand() % 60 + 0x3C;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            }
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 0x1E;
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
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case ONYX_MOVE_CHOICE_KEEP_RANGE:
                if (turn == 0) {
                    entity->move_command = Onyx_GetKeepRangeCommand(entity, 0, 200.0f, 800.0f);
                } else {
                    entity->move_command = Onyx_GetKeepRangeCommand(entity, turn, 200.0f, 800.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, turning to the player throughout;
   from frame EntityParams.near_distance to unk438 it sets flag bit 9 and launches the record's shots on
   two frames in three. */
static void Onyx_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case ACTION_STEP_PLAY:
        EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        if (entity->model->level_tables->frame >= entity->params->near_distance
            && entity->model->level_tables->frame <= entity->params->far_distance) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_LOOK_AT_TARGET;
            if (frame_counter % 3U != 0) {
                EntityAi_LaunchShots(entity, unk3D8, entity->params->near_distance, entity->params->far_distance);
            }
        }
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

/* An action (busy bit 0x1000): plays animation 17 and then animation 18 to their ends. Between the
   two it sets bits 40 and 41 of model parts 8 to 32 and clears them for the parts that go with the
   attack in work byte unk7A (0: parts 22 to 31, 1: 11 and 12, 2: 13 to 21, else 8 to 10), and
   leaves only that attack its weight in work words unk68 to unk6C. At the end it sets work word
   unk6E to EntityParams.action_frames. */
static void Onyx_SwitchAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *part;
    s32 part_index;
    s32 first_part;
    s32 part_count;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames >> 1, 1);
            for (part_index = 0; part_index < 25; part_index++) {
                part = &((EntityModelPart80 *)entity->model->parts)[part_index + 8];
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = TRUE;
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
                ai_state->pause_timer = 0;
                ai_state->sub_timer = 0;
                ai_state->mode_timer = 0;
            }
            switch (ai_state->step_index) {
            case ONYX_ATTACK_RANGED:
                first_part = 0x16;
                part_count = 10;
                ai_state->pause_timer = entity->params->ranged_attacks[0].record.ranged_weight;
                break;
            case ONYX_ATTACK_MELEE_RECORD_1:
                first_part = 0xB;
                part_count = 2;
                ai_state->sub_timer = entity->params->melee_attacks[1].attack_weight;
                break;
            case ONYX_ATTACK_MELEE_RECORD_2:
                first_part = 0xD;
                part_count = 9;
                ai_state->mode_timer = (u8)entity->params->melee_attack_third[0];
                break;
            default:
                first_part = 8;
                part_count = 3;
                break;
            }
            for (part_index = 0; part_index < part_count; part_index++) {
                part = &((EntityModelPart80 *)entity->model->parts)[first_part + part_index];
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = FALSE;
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = FALSE;
            }
            entity->action_step++;
        }
        break;
    case ONYX_SWITCH_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ONYX_SWITCH_STEP_END:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->reload_timer = entity->params->action_frames;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Draws an attack by weight among melee records 1 and 2 (when EntityAi_CanUseMelee finds them in reach)
   and ranged record 0 (EntityAi_CanUseRanged), with the weights in work words unk6A, unk6C and unk68 in
   place of the records' own; sets flag bits 57 to 59 to it and returns whether it is the ranged
   one. With nothing to draw it gives melee record 0. */
static s32 Onyx_PickAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;

    for (i = 0; i < 6; i++) {
        if (i < 3) {
            if (EntityAi_CanUseMelee(entity, i) && (i == 1 || i == 2)) {
                chance = (entity->attack_weights[i] * ((EntityAiStateKind17FC60 *)&entity->ai_state)->action_weights[i]) >> 6;
            } else {
                chance = 0;
            }
        } else {
            if (EntityAi_CanUseRanged(entity, i - 3) && i == 3) {
                chance = (entity->attack_weights[3] * ai_state->pause_timer) >> 6;
            } else {
                chance = 0;
            }
        }
        chances[i] = chance;
        total += chance;
    }
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
        ENTITY_FLAG_BITS(entity)->attack_index = choice;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->attack_index = choice - 3;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & ENTITY_FLAG_ACTIVE_LIST;
}

/* Helper of Onyx_ThinkCombat: the movement command to keep a distance from the player. The
   turn bit follows `turn`, or with 0 the bearing to the target against the turn limit; the
   backwards bit is set nearer than near_range plus 100 plus the radius, the forwards bit farther
   than far_range plus the same. */
static s32 Onyx_GetKeepRangeCommand(Entity *entity, s32 turn, f32 near_range, f32 far_range) {
    f32 angle;
    f32 turn_limit;
    f32 distance;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    if (turn == 0) {
        angle = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->rotation[1], angle);
        turn_limit = entity->params->turn_limit;
        if (0.0f <= angle ? turn_limit < angle : turn_limit < -angle) {
            command = MOVE_COMMAND_TURN_LEFT;
            if (0.0f <= angle) {
                command = MOVE_COMMAND_TURN_RIGHT;
            }
        }
    } else {
        if (turn > 0) {
            command |= MOVE_COMMAND_TURN_RIGHT;
        } else {
            command |= MOVE_COMMAND_TURN_LEFT;
        }
    }
    distance = EntityMath_DistanceXZ(entity->model->position, &player_state.position.x);
    if (distance < near_range + 100.0f + entity->radius) {
        command |= MOVE_COMMAND_BACK;
    } else if (far_range + 100.0f + entity->radius < distance) {
        command |= MOVE_COMMAND_FORWARD;
    }
    return command;
}
