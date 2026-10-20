/* AcidCrawler: the behaviour of entity type 18 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Acid Crawler Large, Acid Crawler
   Small. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
float EntityMath_StepToward(float *, float, float);
extern u32 frame_counter;
int rand(void);
char *Model_GetSkinnedBonePoint(char *, u32);
int EntityAi_LaunchShot(void *, int, void *, void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
int EntityAi_StartAttack(Entity *, void (*)(Entity *entity), int, int, int);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int EntityAi_TurnToPoint(Entity *, f32 *, f32 *);
int Motion_Start(void *, int, int, u32, int);

static void AcidCrawler_Animate(Entity *entity);
static void AcidCrawler_TurnParts30To34ToTarget(void *entity_arg);
static void AcidCrawler_Think(Entity *entity, s32 unknown);
static void AcidCrawler_ThinkCombat(Entity *entity, s32 unknown);
static void AcidCrawler_Move(Entity *entity);

/* Set-up of a type 18 entity: clears its work area and installs the type's handlers. */
void AcidCrawler_Init(Entity *entity) {
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
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = AcidCrawler_Think;
    ai_state->animate = (void (*)(void))AcidCrawler_Animate;
    ai_state->on_hit = (void (*)(void))EntityAi_OnHit;
    ai_state->move = (void (*)(void))AcidCrawler_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
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

/* The animate handler: as EntityAi_Animate, with the head turned at rates 8 and 5 where that one
   has 12 and 8; then AcidCrawler_TurnParts30To34ToTarget turns model parts 30 to 34. */
static void AcidCrawler_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 8.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 5.0f);
        }
        AcidCrawler_TurnParts30To34ToTarget(entity);
    }
}

/* Helper of AcidCrawler_Animate: turns five joints in a row (model parts 30 to 34) towards the
   target, in animations 0, 3, 4, 11 and 12 (in the others it puts them straight). Facing it: each
   joint moves to where the joint before it was, the first back to straight. Otherwise the turn is
   spread over the joints, each taking up to 15 degrees, and the joints past that straighten. */
static void AcidCrawler_TurnParts30To34ToTarget(void *entity_arg) {
    Entity *entity = entity_arg;
    EntityAiStateKind12ACA0 *ai_state = (EntityAiStateKind12ACA0 *)&entity->ai_state;
    EntityModelPart80 *part;
    f32 previous[5];
    f32 limit = 0.2617994f;
    f32 angle;
    f32 step;
    f32 remaining;
    f32 moved;
    s32 done;
    s32 i;
    f32 *previous_joint;
    s32 part_index;

    part = (EntityModelPart80 *)entity->model->parts;
    if (entity->animation_index != 0 && entity->animation_index != 3 && entity->animation_index != 4 && entity->animation_index != 0xB &&
        entity->animation_index != 0xC) {
        ai_state->neck_bend[2] = ai_state->neck_bend[3] = ai_state->neck_bend[4] = ai_state->neck_bend[5] = ai_state->neck_bend[6] = 0.0f;
        return;
    }
    if (entity->target != NULL) {
        angle = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->rotation[1], angle);
        previous[0] = ai_state->neck_bend[2];
        previous[1] = ai_state->neck_bend[3];
        previous[2] = ai_state->neck_bend[4];
        previous[3] = ai_state->neck_bend[5];
        previous[4] = ai_state->neck_bend[6];
        step = entity->params->turn_limit;
        if (angle >= -0.0001f && angle <= 0.0001f) {
            if (ai_state->neck_bend[2] != 0.0f) {
                moved = EntityMath_StepToward(&ai_state->neck_bend[2], 0.0f, step);
                moved = ABS(moved);
                if (moved < step) {
                    step -= moved;
                } else {
                    step = 0.0f;
                }
            }
            previous_joint = previous;
            for (i = 0; i < 4; i++, previous_joint++) {
                if (ai_state->neck_bend[i + 3] != *previous_joint) {
                    moved = EntityMath_StepToward(&ai_state->neck_bend[i + 3], *previous_joint, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        } else {
            remaining = ai_state->turn_rate;
            done = FALSE;
            for (i = 2; i < 7; i++) {
                if (!done) {
                    ai_state->neck_bend[i] += remaining;
                    if (ai_state->neck_bend[i] > limit) {
                        remaining = ai_state->neck_bend[i] - limit;
                        ai_state->neck_bend[i] = limit;
                    } else if (ai_state->neck_bend[i] < -limit) {
                        remaining = ai_state->neck_bend[i] + limit;
                        ai_state->neck_bend[i] = -limit;
                    } else {
                        done = TRUE;
                    }
                } else if (ai_state->neck_bend[i] != 0.0f) {
                    moved = EntityMath_StepToward(&ai_state->neck_bend[i], 0.0f, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        }
    }
    part_index = 30;
    for (i = 0; i < 5; i++) {
        part[part_index++].part_angles[1] = -ai_state->neck_bend[i + 2];
    }
}

/* The think handler: as EntityAi_Think, with AcidCrawler_ThinkCombat for states 1 and 2; after
   EntityAi_ThinkIdle it sets work byte unk7A to 1. Before that, while Entity.ailment_4_frames is 0, it
   launches shot EntityParams.type_param_1 from near the point Model_GetSkinnedBonePoint gives for 0, on two timers:
   work word unk68 (chance EntityParams.action_frames in 100 a frame, then a pause of unk436 frames) and
   work word unk6A (chance unk438, pause type_param_0). */
static void AcidCrawler_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    char *part = Model_GetSkinnedBonePoint((char *)entity->model, 0);
    f32 target[4];
    f32 direction[4];
    f32 offset;

    if (part != NULL && entity->ailment_4_frames == 0) {
        direction[0] = 0.0f;
        direction[1] = 0.0f;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
        if (ai_state->pause_timer == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->action_frames) {
                Vector_Copy(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                EntityAi_LaunchShot(entity, entity->params->type_param_1, target, direction);
                ai_state->pause_timer = entity->params->near_distance;
            }
        }
        if (ai_state->pause_timer != 0) {
            ai_state->pause_timer--;
        }
        if (ai_state->sub_timer == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->far_distance) {
                Vector_Copy(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                EntityAi_LaunchShot(entity, entity->params->type_param_1, target, direction);
                ai_state->sub_timer = entity->params->type_param_0;
            }
        }
        if (ai_state->sub_timer != 0) {
            ai_state->sub_timer--;
        }
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
        break;
    case 1:
    case 2:
        AcidCrawler_ThinkCombat(entity, unknown);
        return;
    }
    ((void (*)(void *, int))EntityAi_ThinkIdle)(entity, unknown); // match: called as void, $2 is reused at once
    ai_state->step_index = 1;
}

/* The think function of states 1 and 2: guards or dodges the player's swing as EntityAi_ThinkCombat
   does; attacks with what EntityAi_PickAttack draws once the pause Entity.attack_delay_frames is over and EntityAi_CanAttack
   finds the player in reach; each time flag bit 32 comes on it starts EntityAi_RangedAttack with
   record 0, 40 times in 100 (work byte unk7A); else it goes for the player with command 0x841, or
   0x241 / 0x141 while it is pushed. */
static void AcidCrawler_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
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
    if (entity->attack_delay_frames == 0 && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) { // match: the entity is passed
        entity->target = NULL;
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
        }
        return;
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    if ((s32)(entity->flags >> 32) & 1) {
        if (ai_state->step_index == 1) {
            ai_state->step_index = 0;
            if (rand() % 100 < 40) {
                EntityAi_StartAttack(entity, EntityAi_RangedAttack, unknown, 1, 0);
                return;
            }
        }
    } else {
        ai_state->step_index = 1;
    }
    EntityAi_ClearTurnInPlace(entity);
    if (ENTITY_FLAG_BITS(entity)->bit31) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ai_state->turn_in_place == 0) {
            ai_state->turn_in_place = ABS(angle) > 0.5235988f;
            if (ai_state->turn_in_place == 0) {
                goto done;
            }
        }
        entity->move_command = EntityAi_TurnCommand(entity);
        if (entity->move_command == 0) {
            ai_state->turn_in_place = 0;
        }
    } else if (ai_state->move_choice == 1) {
        entity->target = &player_state;
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_TO_TARGET);
        }
    } else {
        ai_state->move_choice = 1;
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The move handler: turns as the command says, or towards the target with command bit 0x800
   (EntityAi_TurnToPoint); picks animation 4 or 3 walking, 3 turning, 17 for command bit 0x8000, 4 for bit
   0x800, else 0. It adds no movement of its own. */
static void AcidCrawler_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 vector[4];
    f32 turn;

    if (entity == player_state.stood_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    vector[0] = vector[1] = vector[2] = vector[3] = turn = 0.0f;
    if (entity->move_command & MOVE_COMMAND_TURN_TO_TARGET) {
        if (entity->target != NULL) {
            EntityAi_TurnToPoint(entity, &ai_state->turn_rate, entity->target);
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
        turn = ai_state->turn_rate;
        if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
            turn += turn;
        }
        entity->turn_delta_yaw += turn;
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (entity->params->turn_limit < ai_state->turn_rate) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
        turn = ai_state->turn_rate;
        if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
            turn += turn;
        }
        entity->turn_delta_yaw += turn;
    } else {
        ai_state->turn_rate = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, (s32)((f32)entity->params->motion_blend_frames * 0.5f), 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_8000) {
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURN_TO_TARGET) {
        Motion_Start(entity, 4, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}
