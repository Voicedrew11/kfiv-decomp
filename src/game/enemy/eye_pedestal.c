/* EyePedestal: the behaviour of entity type 31 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: chr.prm record 91, model
   c0091 (an eye set in a stone pedestal), whose eight spawns stand in two rows of four in 006.map. They
   neither move nor take hits; each turns its eye after the player, except the one spawned with
   EntitySpawnInfo.spawn_mode set. Prima's official guide (2004) describes the room: eight pedestals with eyes
   that follow you, and the one that does not is the switch that closes them all and stops the ceiling
   trap of the next room. The game itself names them nowhere. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
int Motion_Start(void *, int, int, u32, int);
extern PlayerData player_state;

static void EyePedestal_Animate(Entity *entity);
static void EyePedestal_Think(Entity *entity, s32 run_now);

/* Set-up of a type 31 entity: clears its work area and installs the type's handlers. */
void EyePedestal_Init(Entity *entity) {
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
    ai_state->think = EyePedestal_Think;
    ai_state->animate = EyePedestal_Animate;
    ai_state->on_hit = NULL;
    ai_state->move = NULL;
    ai_state->dormant = NULL;
    ai_state->death = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and with flag
   bit 9 set the head follows the point in work floats unk48 to unk50 (EntityAi_LookAtPoint, at the rate
   EntityParams.action_frames) instead of the target. */
static void EyePedestal_Animate(Entity *entity) {
    f32 limits[4];
    f32 point[3];
    u32 blend_frames; // match: converted to float as unsigned
    s32 look_at_target;
    EntityParams *params;
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        params = entity->params;
        limits[0] = DEG_TO_RAD(-params->pitch_limit_deg);
        limits[1] = DEG_TO_RAD(params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-params->yaw_limit_deg);
        point[0] = ai_state->part_angle_x;
        point[1] = ai_state->part_angle_y;
        point[2] = ai_state->part_angle_z;
        look_at_target = ENTITY_FLAG_BITS(entity)->look_at_target;
        if (look_at_target) {
            EntityAi_LookAtPoint(entity, (s32)ai_state->look_part_angles, look_at_target, (s32)point, (s32)limits, (f32)(s16)params->action_frames);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* The think handler (it takes the entity only): no states and no actions. Every EntityParams.near_distance
   frames (counted in work byte unk7A) it puts the player's position, 220 less in y, in work floats
   unk48 to unk50. It keeps flag bits 16, 18 and 21 set, flag bit 9 set unless EntitySpawnInfo.spawn_mode
   is, and plays animation 0 while flag bit 29 is set and animation 1 otherwise. */
static void EyePedestal_Think(Entity *entity, s32 run_now) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    if (ai_state->think_flag != 0) {
        ai_state->think_flag--;
    } else {
        ai_state->point_x = player_state.position[0];
        ai_state->point_y = player_state.position[1] - 220.0f;
        ai_state->point_z = player_state.position[2];
        ai_state->think_flag = (u8)entity->params->near_distance;
    }
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
    if (entity->spawn_info->spawn_mode != 0) {
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit29) {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 0);
    }
}
