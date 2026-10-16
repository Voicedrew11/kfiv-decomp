/* EyePedestal: the behaviour of entity type 31 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: chr.prm record 91, model
   c0091 (an eye set in a stone pedestal), whose eight spawns stand in two rows of four in 006.map. They
   neither move nor take hits; each turns its eye after the player, except the one spawned with
   EntitySpawnInfo.spawn_mode set. Prima's official guide (2004) describes the room: eight pedestals with eyes
   that follow you, and the one that does not is the switch that closes them all and stops the ceiling
   trap of the next room. The game itself names them nowhere. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
int Motion_Start(void *, int, int, u32, int);
extern PlayerData player_state;

static void EyePedestal_Animate(Entity *entity);
static void EyePedestal_Think(Entity *entity);

/* Set-up of a type 31 entity: clears its work area and installs the type's handlers. */
void EyePedestal_Init(Entity *entity) {
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
    unkE0->think = (void (*)(Entity *, s32))EyePedestal_Think;
    unkE0->animate = (void (*)(void))EyePedestal_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = NULL;
    unkE0->dormant = NULL;
    unkE0->death = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (unk138 != NULL) {
        Vector_Copy(unkE0->look_part_angles, unk138 + 0x50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and with flag
   bit 9 set the head follows the point in work floats unk48 to unk50 (EntityAi_LookAtPoint, at the rate
   EntityParams.action_frames) instead of the target. */
static void EyePedestal_Animate(Entity *entity) {
    f32 limits[4];
    f32 point[3];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    s32 look_at_target;
    EntityParams *params;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->level_tables->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & 0x12) ? unkE0->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        params = entity->params;
        limits[0] = -params->pitch_limit_deg * 0.017453292f;
        limits[1] = params->pitch_limit_deg * 0.017453292f;
        limits[3] = params->yaw_limit_deg * 0.017453292f;
        limits[2] = -params->yaw_limit_deg * 0.017453292f;
        point[0] = unkE0->part_angle_x;
        point[1] = unkE0->part_angle_y;
        point[2] = unkE0->part_angle_z;
        look_at_target = (s32)(entity->flags >> 9) & 1;
        if (look_at_target) {
            EntityAi_LookAtPoint(entity, (s32)unkE0->look_part_angles, look_at_target, (s32)point, (s32)limits, (f32)(s16)params->action_frames);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* The think handler (it takes the entity only): no states and no actions. Every EntityParams.near_distance
   frames (counted in work byte unk7A) it puts the player's position, 220 less in y, in work floats
   unk48 to unk50. It keeps flag bits 16, 18 and 21 set, flag bit 9 set unless EntitySpawnInfo.spawn_mode
   is, and plays animation 0 while flag bit 29 is set and animation 1 otherwise. */
static void EyePedestal_Think(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (unkE0->think_flag != 0) {
        unkE0->think_flag--;
    } else {
        unkE0->point_x = player_state.position[0];
        unkE0->point_y = player_state.position[1] - 220.0f;
        unkE0->point_z = player_state.position[2];
        unkE0->think_flag = (u8)entity->params->near_distance;
    }
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    if (entity->spawn_info->spawn_mode != 0) {
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit29) {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 0);
    }
}
