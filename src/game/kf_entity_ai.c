/* The shared entity AI helpers: handler switching, attack reach tests, turning, the attack
   delay, the motion slots of the work area. */
#include "common.h"
#include "kf_entity.h"
#include "kf_shot.h"
#include "kf_effect.h"
#include "kf_player.h"
#include "kf_map.h"
#include "kf_model.h"
#include "kf_entity_ai.h"

int rand(void);
s32 EntitySight_PropInWay(Entity *entity);
s32 EntitySight_ObjectInWay(Entity *entity);
#include "kf_gs.h"

extern float D_004325A0[];
s32 EntityAim_AilPlayerInArc(void *target, f32 *point, f32 max_angle, f32 max_height, f32 max_distance, f32 heading);
void *memset(void *, int, u32);
void Vector_Copy(void *, void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);
/* EntityMath_WrapAngle, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}
int func_001B4F30(u16, void *, void *, void *, void *);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
s32 Bone_TurnPart(ModelPart *part, f32 *angles, s32 mode, f32 *goal, f32 *limits, f32 rate);
/* match: the call leaves the sixth argument (rate, $f12) unset */
#define TURN_PART_NO_RATE ((int (*)(void *, f32 *, int, f32 *, f32 *))Bone_TurnPart)
int Bone_TurnPartToLook(void *model, int part_arg, int angles_arg, int mode, void *target, int limits_arg, float rate);
extern PlayerState D_00413F80;
#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->unk38)
extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
int func_00103548(int, int);
// match: reading EntityParams word 0 whole; a bit-field in byte 3 would be an lbu
typedef union {
    u32 word;
} EntityParamsFirstWord;
#define ENTITY_PARAMS_WORD(params) (((EntityParamsFirstWord *)(params))->word)
void EntityCollision_PushApart(Entity *);
void EntityCollision_WalkAgainstMap(Entity *);
void EntityCollision_SphereAgainstMap(Entity *);
void EntityCollision_WalkAgainstObjects(Entity *);
void EntityCollision_SphereAgainstObjects(Entity *);
extern u8 item_counts[ITEM_COUNT];
/* Moves *level by delta, keeping it between 0x40 and max.
   match: max is a parameter so the 0xC0 cap stays in a register (slt/movn), as in EntityAi_RaiseAttackWeight */
static inline void adjust_level(u8 *level, s32 delta, s32 max) {
    s32 value = *level + delta;

    if (value < 0x40) {
        value = 0x40;
    }
    if (value > max) {
        value = max;
    }
    *level = value;
}
int Motion_SeekSlotKeys(ModelInstance *, int, int, int);
s32 Motion_BlendSlotToPose(ModelInstance *model, s32 type, s32 table, s16 frame, f32 weight);
void Vector_WeightedSum4(void *, void *, void *, void *, void *, float *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_0010A718(int);
void Matrix_Multiply(void *, void *, void *);
extern Entity *D_004467E0;
extern ModelInstance D_003BE530[];
extern char D_003E74C0[];

static s32 func_00195990(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame_min, s32 frame_max);
static void EntityAi_Kill(Entity *entity);
static int func_00196078(Entity *actor, f32 *out_a, int mode, f32 *out_b, f32 *limit);
static int func_001961D0(Entity *entity, f32 *angles, int mode, f32 *goal, f32 *limits);
static int func_00196200(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale);
static int func_00196248(Entity *entity, int angles, int mode, int limits, float rate);
static int func_00196358(Entity *entity);
static void func_00196548(Entity *actor);
static void func_00196718(Entity *actor);
static int EntityAi_HasMeleeAttack(Entity *entity);
static int EntityAi_HasRangedAttack(Entity *entity);
static s32 EntityAi_InAttackDistance(Entity *entity);
static int EntityAi_InMeleeDistance(Entity *entity);
static int EntityAi_InMeleeDistanceOf(Entity *entity, int index);
static int EntityAi_InRangedDistance(Entity *entity);
static int EntityAi_InRangedDistanceOf(Entity *entity, int index);
static float EntityAi_Decelerate(Entity *entity, float speed);

/* A random whole number of degrees as an angle in radians, wrapped to -pi..pi. */
float EntityAi_RandomAngle(void) {
    float angle;

    angle = (float) (rand() % 360) * 0.017453292f;
    if (3.1415927f <= angle) {
        angle -= 6.2831855f;
    }
    return angle;
}

/* Sets D_004325A0[30] to float_value and D_004325A0[31] to int_value. */
void func_00195878(int float_value, int int_value) {
    D_004325A0[30] = float_value;
    *(int *)&D_004325A0[31] = int_value;
}

/* Sets Entity.state and keeps the old value in prev_state. */
void EntityAi_SetState(Entity *entity, u8 state) {
    u8 old_state;

    old_state = entity->state;
    entity->state = state;
    entity->prev_state = old_state;
}

/* Installs the entity's per-frame update (Entity.unk50; the type table D_003E6E40 gives it) and puts
   action_step back to 0. */
void EntityAi_SetUpdate(Entity *entity, s32 update) {
    entity->unk50 = update;
    entity->action_step = 0;
}

/* Switches the entity's handler, keeping the old one as prev_handler; optionally runs the new one now. */
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now) {
    entity->action_step = 0;
    entity->prev_handler = entity->handler;
    entity->handler = handler;
    if (run_now != 0 && handler != NULL) {
        handler(entity);
    }
    return 1;
}

/* Switches the entity to handler (keeping the old one), sets flag bit 57 and bits 58-59, optionally runs it. */
int EntityAi_StartAttack(Entity *entity, void (*handler)(Entity *entity), int run, int bit57, int bits58) {
    ENTITY_FLAG_BITS(entity)->bit57 = bit57;
    ENTITY_FLAG_BITS(entity)->bits58 = bits58;
    entity->action_step = 0;
    entity->prev_handler = entity->handler;
    entity->handler = handler;
    if (run != 0 && handler != NULL) {
        handler(entity);
    }
    return 1;
}

/* As EntityAi_CheckMeleeHit, but hands the part to EntityAim_AilPlayerInArc, which tests the player alone (heading
   turned round by pi). Nothing calls it. */
static s32 func_00195990(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame_min, s32 frame_max) {
    EntityModel *model;
    f32 turn;
    f32 wrapped;

    model = entity->model;
    turn = record->unk0E * 0.017453292f;
    if (!(0.0f <= model->unkE0[0])) {
        turn = -turn;
    }
    wrapped = model->unkD0[1] + turn + 3.1415927f;
    wrapped = wrapped - (f32)(s32)(wrapped / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    if ((frame_min < record->unk10[index].unk24 && frame_max < record->unk10[index].unk24) || (record->unk10[index].unk26 < frame_min && record->unk10[index].unk26 < frame_max)) {
        return 0;
    }
    return EntityAim_AilPlayerInArc(&record->unk10[index].unk12, model->position, record->unk10[index].unk22 * 0.017453292f, entity->unk34, record->unk10[index].unk20, wrapped);
}

/* Hit check of one part of a melee record: turns the model's heading by the record's angle (mirrored when
   the model is) and hands it to EntityAi_CheckMeleeHitAt. */
int EntityAi_CheckMeleeHit(Entity *entity, EntityParamsUnk240 *record, int part, int frame_from, int frame_to) {
    ModelInstance *model = ENTITY_MODEL(entity);
    float delta = (float)record->unk0E * 0.017453292f;
    float angle;

    if (!(model->unkE0 >= 0.0f)) {
        delta = -delta;
    }
    angle = model->rotation.y + delta;
    angle = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return EntityAi_CheckMeleeHitAt(entity, record, part, frame_from, frame_to, angle);
}

/* Runs EntityAi_SendMeleeHit for one part of a melee record facing angle, unless the animation frames
   [frame_from, frame_to] lie outside the frames the part is active on (0 then). */
s32 EntityAi_CheckMeleeHitAt(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
    if (unk240->unk10[part].unk24 == 0 ||
        (frame_from < unk240->unk10[part].unk24 && frame_to < unk240->unk10[part].unk24) ||
        (unk240->unk10[part].unk26 < frame_from && unk240->unk10[part].unk26 < frame_to)) {
        return 0;
    }
    return EntityAi_SendMeleeHit(entity, unk240, part, angle);
}

/* Builds the attack request of one part of a melee record (reach, height, angle, power) and runs it with
   func_001B1470; returns its result. */
s32 EntityAi_SendMeleeHit(Entity *entity, EntityParamsUnk240 *record, s32 index, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;

    memset(&request, 0, sizeof(request));
    memset(&extra, 0, sizeof(extra));
    Vector_Copy(request.position, entity->model->position);
    yaw = angle + 3.1415927f;
    request.unk30 = entity->radius + record->unk10[index].unk20;
    request.unk34 = entity->height + 10.0f;
    request.unk38 = wrap_angle(yaw);
    request.unk3C = wrap_angle(record->unk10[index].unk22 * 0.017453292f);
    request.owner = entity;
    request.unk4C_0 = 3;
    request.unk4D = 1;
    request.unk50 = record->unk10[index].unk00;
    request.unk62 = record->unk10[index].unk12;
    request.unk4E = record->unk0C;
    request.unk4C_3 = record->unk10[index].unk1E & 1;
    if ((ENTITY_PART_FLAGS(&record->unk10[index])->flags >> 1) & 1) {
        request.unk71 = record->unk10[index].unk1F;
    }
    extra.unk00 = 0;
    extra.unk02 = 0;
    extra.unk20 = 0;
    extra.unk22 = 0;
    return func_001B1470(&request, &extra);
}

/* Allocates a shot carrying EntityParams.unk238 and launches effect effect_id from origin along angles
   (func_001B4F30) for the entity. */
int EntityAi_LaunchShot(Entity *entity, int effect_id, void *origin, void *angles) {
    EntityUnk1B1390 *shot;

    shot = Shot_Alloc();
    shot->unk22 = shot->unk20 = 0;
    shot->unk00 = 1;
    shot->unk28 = entity->params->unk238;
    return func_001B4F30((u16)effect_id, shot, origin, angles, entity);
}

/* EntityShot_LaunchAimed with no target: launches the shots of a ranged record whose frame lies in (frame_from,
   frame_to]. */
int EntityAi_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 elapsed, s32 unk82) {
    return EntityShot_LaunchAimed(entity, unk3D8, elapsed, unk82, NULL, 0.0f, 0.0f);
}

/* Spawns an effect at a model point, facing along the line to the next point. */
void EntityAi_SpawnBoneEffect(char **model, int effect_id, int point) {
    f32 *from;
    f32 *tip;
    f32 angles[4];
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 yaw;
    f32 turns;

    from = (f32 *)func_0010CAB8(*model, point);
    tip = (f32 *)func_0010CAB8(*model, point + 1);
    if (from != NULL && tip != NULL) {
        delta_x = tip[0] - from[0];
        delta_y = tip[1] - from[1];
        delta_z = tip[2] - from[2];
        angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
        yaw = -atan2f(delta_z, delta_x) + 1.5707964f;
        turns = (int)(yaw / 6.2831855f);
        yaw -= turns * 6.2831855f;
        if (3.1415927f < yaw) {
            yaw -= 6.2831855f;
        } else if (yaw < -3.1415927f) {
            yaw += 6.2831855f;
        }
        angles[1] = yaw;
        angles[2] = angles[3] = 0.0f;
        Effect_StartSpawner(effect_id, from, angles, model);
    }
}

/* Sets Entity.unk2C (its health) to 0, so EntityAi_Update starts the death action. Nothing calls it by name. */
static void EntityAi_Kill(Entity *entity) {
    entity->unk2C = 0;
}

/* Bone_TurnPart for the model part at EntityModel.unk138 while params flag 0x2000 is set; FALSE without it.
   Nothing calls it. */
static int func_00196078(Entity *actor, f32 *out_a, int mode, f32 *out_b, f32 *limit) {
    void *target = actor->model->unk138;

    if (target == NULL) {
        return 0;
    }
    if (!(ENTITY_CONTROLLER(actor)->flags & 0x2000)) {
        return 0;
    }
    return TURN_PART_NO_RATE(target, out_a, mode, out_b, limit);
}

/* Turns the model part at EntityModel.unk138 to look at target (Bone_TurnPartToLook) while params flag 0x2000 is
   set; FALSE without it. */
int EntityAi_LookAtPoint(Entity *entity, int angles, int mode, int target, int limits, float rate) {
    EntityModel *model = entity->model;
    int part = (int)model->unk138;

    if (part == 0 || !(ENTITY_CONTROLLER(entity)->flags & 0x2000)) {
        return 0;
    }
    return Bone_TurnPartToLook(model, part, angles, mode, (void *)target, limits, rate);
}

/* As EntityAi_LookAtPoint, at the player's position 220 above his feet. */
int EntityAi_LookAtPlayer(Entity *entity, void *target, s32 mode, void *out, f32 range) {
    f32 position[4];
    int result;

    Vector_Copy(position, &D_00413F80.position);
    position[1] -= 220.0f;
    if (entity->model->unk138 == NULL || !(entity->params->flags & 0x2000)) {
        result = 0;
    } else {
        result = Bone_TurnPartToLook(entity->model, (int)entity->model->unk138, (int)target, mode, position, (int)out, range);
    }
    return result;
}

/* Bone_TurnPart for the model part at EntityModel.unk134; FALSE without it. Nothing calls it. */
static int func_001961D0(Entity *entity, f32 *angles, int mode, f32 *goal, f32 *limits) {
    int part = entity->model->unk134;
    int result;

    if (part == 0) {
        result = 0;
    } else {
        result = TURN_PART_NO_RATE((void *)part, angles, mode, goal, limits);
    }
    return result;
}

/* Bone_TurnPartToLook for the model part at EntityModel.unk134; FALSE without it. Nothing calls it. */
static int func_00196200(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale) {
    EntityModel *model = entity->model;

    if (model->unk134 == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->unk134, value_a, value_b, value_c, value_d, scale);
}

/* Bone_TurnPartToLook for the model part at EntityModel.unk134, aimed 115 above the player's feet; FALSE without
   it. Nothing calls it. */
static int func_00196248(Entity *entity, int angles, int mode, int limits, float rate) {
    float position[2]; // match: Vector_Copy writes four floats, but retail reserves room for two
    EntityModel *model;

    Vector_Copy(position, &D_00413F80.position);
    position[1] -= 115.0f;
    model = entity->model;
    if (model->unk134 == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->unk134, angles, mode, position, limits, rate);
}

/* Once the model is below y 10000 (fallen out of the map): frees its effects, clears flag bit 1 and returns
   1; else 0. */
s32 EntityAi_RemoveIfFallen(Entity *entity) {
    u16 ctrl_flags;

    if (*(f32 *)((char *)entity->model + 0xC4) > 10000.0f) {
        ctrl_flags = ENTITY_CONTROLLER(entity)->unkC6;
        if (ctrl_flags != 0 && ctrl_flags != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->flags &= ~2;
        return 1;
    }
    return 0;
}

/* 1 when the player is within 8000, unless unk14 is set and its spawn flags lack 0x04000000 (0 then).
   Nothing calls it. */
static int func_00196358(Entity *entity) {
    if (entity->unk14 != NULL && (((EntitySpawnInfo *)entity->unk14)->unk38 & 0x04000000) == 0) {
        return 0;
    }
    if (entity->distance_to_player <= 8000.0f) {
        return 1;
    }
    return 0;
}

/* Decides whether a dead entity comes back: it must be allowed to, have deaths left, and the player must
   have come within its respawn distance, at which point one roll against its respawn chance is made. */
s32 EntityAi_ShouldRespawn(Entity *entity) {
    f32 distance[4];
    EntitySpawnInfo *spawn_info;
    s32 death_count;

    if (!ENTITY_FLAG_BITS(entity)->bit7) {
        return FALSE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit4 && (entity->spawn_info->unk38 & (0x8000L << 46))) {
        return FALSE;
    }
    death_count = func_00103548(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
    if ((s8)SPAWN_FLAGS(entity->spawn_info)->bytes[5] != 0 && death_count >= (s8)SPAWN_FLAGS(entity->spawn_info)->bytes[5]) {
        return FALSE;
    }
    spawn_info = entity->spawn_info;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%3)\n"
        : "=m"(distance) : "r"(&spawn_info->spawn_x), "r"(&D_00413F80.position), "r"(distance));
    if (distance[0] <= (f32)spawn_info->unk50) {
        if ((entity->spawn_info->unk38 & (0xC000L << 45)) == (0x8000L << 44)) {
            return TRUE;
        }
        if (ENTITY_FLAG_BITS(entity)->pad8) {
            return FALSE;
        }
        ENTITY_FLAG_BITS(entity)->pad8 = TRUE;
        if (rand() % 100 < entity->spawn_info->unk52) {
            return TRUE;
        }
    } else {
        ENTITY_FLAG_BITS(entity)->pad8 = FALSE;
    }
    return FALSE;
}

/* The flag-32 half of EntityAi_SensePlayer on its own. Nothing calls it. */
static void func_00196548(Entity *actor) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 reach = ctrl->unk18;
    f32 half_height = ctrl->unk1A;
    f32 arc = ctrl->unk1C;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    turn = actor->unk68 - actor->model->unkD0[1];
    turn -= (s32)(turn / 6.2831855f) * 6.2831855f;
    if (3.1415927f < turn) {
        turn -= 6.2831855f;
    } else if (turn < -3.1415927f) {
        turn += 6.2831855f;
    }
    turn_abs = 0.0f <= turn ? turn : -turn;
    upper = actor->model->position[1] - (actor->height + half_height) * 0.5f;
    lower = actor->model->position[1] - (actor->height - half_height) * 0.5f;
    ENTITY_FLAG_BITS(actor)->bit32 = 0;
    if (actor->distance_to_player < actor->radius + 100.0f + reach && turn_abs < arc && upper <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= lower) {
        ENTITY_FLAG_BITS(actor)->bit32 = 1;
    }
}

/* The flag-33 half of EntityAi_SensePlayer on its own. Nothing calls it. */
static void func_00196718(Entity *actor) {
    f32 reach = ENTITY_CONTROLLER(actor)->unk18;
    f32 limit;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + reach;
    if (actor->flags & ((u64)0x20001 << 32)) {
        actor->flags |= (u64)0x8000 << 18;
    } else if (limit < actor->distance_to_player) {
        actor->flags &= ~((u64)1 << 33);
    } else {
        s32 chance = (s32)((limit - actor->distance_to_player) * 100.0f / limit) * 2;

        if (rand() % 100 < chance) {
            actor->flags |= (u64)0x8000 << 18;
        }
    }
}

/* Sets flag 32 while the player is within reach, arc and height, and flag 33 (noticed) by a chance that
   grows as he comes nearer; flag 63 when something stands between them, which clears flag 32 (mode below 2)
   and flag 33 (mode 0). */
void EntityAi_SensePlayer(Entity *actor, s32 mode) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 half_height = ctrl->unk1A;
    f32 reach = ctrl->unk18;
    f32 arc = ctrl->unk1C;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;
    f32 near_reach;
    f32 limit;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    turn = actor->unk68 - actor->model->unkD0[1];
    turn -= (s32)(turn / 6.2831855f) * 6.2831855f;
    if (3.1415927f < turn) {
        turn -= 6.2831855f;
    } else if (turn < -3.1415927f) {
        turn += 6.2831855f;
    }
    turn_abs = 0.0f <= turn ? turn : -turn;
    upper = actor->model->position[1] - (actor->height + half_height) * 0.5f;
    lower = actor->model->position[1] - (actor->height - half_height) * 0.5f;
    ENTITY_FLAG_BITS(actor)->bit32 = 0;
    if (actor->distance_to_player < actor->radius + 100.0f + reach && turn_abs < arc && upper <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= lower) {
        ENTITY_FLAG_BITS(actor)->bit32 = 1;
    }
    near_reach = ctrl->unk18;
    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        near_reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        near_reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + near_reach;
    if (actor->flags & ((u64)0x20001 << 32)) {
        actor->flags |= (u64)0x8000 << 18;
    } else if (limit < actor->distance_to_player) {
        actor->flags &= ~((u64)1 << 33);
    } else {
        s32 chance = (s32)((limit - actor->distance_to_player) * 100.0f / limit) * 2;

        if (rand() % 100 < chance) {
            actor->flags |= (u64)0x8000 << 18;
        }
    }
    actor->flags &= ~((u64)1 << 63);
    if (EntitySight_PropInWay(actor) - 1 < 3U || EntitySight_ObjectInWay(actor) != 0) {
        actor->flags |= (u64)0x8000 << 48;
        if (mode < 2) {
            actor->flags &= ~((u64)1 << 32);
            if (mode == 0) {
                actor->flags &= ~((u64)1 << 33);
            }
        }
    }
}

/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.unk4C (in x, y and z),
   clears it otherwise. */
void EntityAi_CheckFarFromSpawn3D(Entity *entity) {
    EntitySpawnInfo *spawn_info;
    EntityModel *model;
    float delta_x;
    float delta_y;
    float delta_z;
    u16 limit;

    spawn_info = entity->spawn_info;
    model = entity->model;
    delta_x = model->position[0] - spawn_info->spawn_x;
    delta_y = model->position[1] - spawn_info->spawn_y;
    limit = spawn_info->unk4C;
    delta_z = model->position[2] - spawn_info->spawn_z;
    if ((float)(limit * limit) < (delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z)) {
        entity->flags |= (0x8000UL << 38);
        return;
    }
    entity->flags &= 0xFFDFFFFFFFFFFFFFUL;
}

/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.unk4C (in x and z),
   clears it otherwise. */
void EntityAi_CheckFarFromSpawn(Entity *entity) {
    EntitySpawnInfo *spawn_info;
    EntityModel *model;
    float delta_x;
    float delta_z;
    u16 limit;

    spawn_info = entity->spawn_info;
    model = entity->model;
    delta_x = model->position[0] - spawn_info->spawn_x;
    limit = spawn_info->unk4C;
    delta_z = model->position[2] - spawn_info->spawn_z;
    if ((float)(limit * limit) < (delta_x * delta_x) + (delta_z * delta_z)) {
        entity->flags |= (0x8000UL << 38);
        return;
    }
    entity->flags &= 0xFFDFFFFFFFFFFFFFUL;
}

/* Clears flag bit 31. */
void func_00196D48(Entity *entity) {
    entity->flags &= 0xFFFFFFFF7FFFFFFFULL;
}

/* Tests whether the target is within a distance and inside the horizontal and vertical view cones (each
   given as a full angle); a limit of 0 skips that test. */
s32 EntityAi_IsPointInView(Entity *entity, f32 *target, f32 yaw_range, f32 pitch_range, f32 max_distance) {
    f32 distance[4];
    f32 angle;
    f32 diff;
    f32 magnitude;

    if (max_distance != 0.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%1)\n"
            "lqc2 $vf17, 0(%2)\n"
            "vsub.xyz $vf16, $vf16, $vf17\n"
            "vmul.xyz $vf16, $vf16, $vf16\n"
            "vaddy.x $vf16, $vf16, $vf16y\n"
            "vaddz.x $vf16, $vf16, $vf16z\n"
            "vsqrt Q, $vf16x\n"
            "vwaitq\n"
            "vaddq.x $vf16, $vf0, Q\n"
            "sqc2 $vf16, 0(%3)\n"
            : "=m"(distance) : "r"(entity->model->position), "r"(target), "r"(distance));
        if (max_distance < distance[0]) {
            return 0;
        }
    }
    if (yaw_range != 0.0f) {
        angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
        angle += 3.1415927f;
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        angle -= entity->model->unkD0[1];
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        diff = angle;
        magnitude = 0.0f <= diff ? diff : -diff;
        if (yaw_range * 0.5f < magnitude) {
            return 0;
        }
    }
    if (pitch_range != 0.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%1)\n"
            "lqc2 $vf17, 0(%2)\n"
            "vsub.xyz $vf16, $vf16, $vf17\n"
            "vmul.xz $vf16, $vf16, $vf16\n"
            "vaddz.x $vf16, $vf16, $vf16z\n"
            "vsqrt Q, $vf16x\n"
            "vwaitq\n"
            "vaddq.x $vf16, $vf0, Q\n"
            "sqc2 $vf16, 0(%3)\n"
            : "=m"(distance) : "r"(entity->model->position), "r"(target), "r"(distance));
        diff = atan2f(target[1] - entity->model->position[1], distance[0]);
        diff -= entity->model->unkD0[0];
        diff = diff - (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
        if (3.1415927f < diff) {
            diff -= 6.2831855f;
        } else if (diff < -3.1415927f) {
            diff += 6.2831855f;
        }
        magnitude = 0.0f <= diff ? diff : -diff;
        if (pitch_range * 0.5f < magnitude) {
            return 0;
        }
    }
    return 1;
}

/* Nonzero when any of the three melee records has a weight. */
static int EntityAi_HasMeleeAttack(Entity *entity) {
    EntityParams *params = entity->params;

    if (params->unk240[0].unk00 != 0 || params->unk240[1].unk00 != 0 || (u8)params->unk350[0] != 0) {
        return 1;
    }
    return 0;
}

/* Nonzero when any of the three ranged records has a weight. */
static int EntityAi_HasRangedAttack(Entity *entity) {
    EntityParams *params = entity->params;

    if (params->unk3D8[0].record.unk00 != 0 || ENTITY_PARAMS_UNK3D8(params, 1)->unk00 != 0 || ENTITY_PARAMS_UNK3D8(params, 2)->unk00 != 0) {
        return 1;
    }
    return 0;
}

/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three melee or
   three ranged attacks. */
s32 EntityAi_CanAttack(Entity *entity) {
    f32 angle_off[6];
    f32 reach[12]; /* nearest per attack, then farthest per attack */
    f32 range;
    f32 top;
    f32 bottom;

    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[0] = heading;
            magnitude = angle_off[0];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[0] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[1] = heading;
            magnitude = angle_off[1];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[1] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[2] = heading;
            magnitude = angle_off[2];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[2] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[0].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[3] = heading;
            magnitude = angle_off[3];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[3] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[1].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[4] = heading;
            magnitude = angle_off[4];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[4] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[2].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[5] = heading;
            magnitude = angle_off[5];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[5] = magnitude;
        }
    }
    top = entity->model->position[1] - (entity->height + entity->params->unk1A) * 0.5f;
    bottom = entity->model->position[1] - (entity->height - entity->params->unk1A) * 0.5f;
    range = entity->params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        range *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    reach[3] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    reach[4] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    reach[5] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    reach[6] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06) + entity->radius + 100.0f;
    reach[7] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06) + entity->radius + 100.0f;
    reach[8] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06) + entity->radius + 100.0f;
    reach[9] = (range < entity->params->unk3D8[0].reach_max ? range : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    reach[10] = (range < entity->params->unk3D8[1].reach_max ? range : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    reach[11] = (range < entity->params->unk3D8[2].reach_max ? range : entity->params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    if (entity->model->position[1] - entity->height <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= entity->model->position[1]) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk00 != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk08 && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[6]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk00 != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk08 && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[7]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk00 != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk08 && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[8]) {
            return 1;
        }
    }
    if (top <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= bottom) {
        if (entity->params->unk3D8[0].record.unk00 != 0 && angle_off[3] < entity->params->unk3D8[0].unk08 && reach[3] <= entity->distance_to_player && entity->distance_to_player < reach[9]) {
            return 1;
        }
        if (entity->params->unk3D8[1].record.unk00 != 0 && angle_off[4] < entity->params->unk3D8[1].unk08 && reach[4] <= entity->distance_to_player && entity->distance_to_player < reach[10]) {
            return 1;
        }
        if (entity->params->unk3D8[2].record.unk00 != 0 && angle_off[5] < entity->params->unk3D8[2].unk08 && reach[5] <= entity->distance_to_player && entity->distance_to_player < reach[11]) {
            return 1;
        }
    }
    return 0;
}

/* Whether the player is at a distance one of the entity's six attacks (three melee, three ranged) is used
   at. */
static s32 EntityAi_InAttackDistance(Entity *entity) {
    f32 reach[12]; /* nearest, then farthest, per attack */
    f32 range;
    f32 distance;

    range = entity->params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        range *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    reach[3] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    reach[4] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    reach[5] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    reach[6] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06) + entity->radius + 100.0f;
    reach[7] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06) + entity->radius + 100.0f;
    reach[8] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06) + entity->radius + 100.0f;
    reach[9] = (range < entity->params->unk3D8[0].reach_max ? range : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    reach[10] = (range < entity->params->unk3D8[1].reach_max ? range : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    reach[11] = (range < entity->params->unk3D8[2].reach_max ? range : entity->params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    distance = entity->distance_to_player;
    if ((reach[0] <= distance && distance < reach[6]) ||
        (reach[1] <= distance && distance < reach[7]) ||
        (reach[2] <= distance && distance < reach[8]) ||
        (reach[3] <= distance && distance < reach[9]) ||
        (reach[4] <= distance && distance < reach[10]) ||
        (reach[5] <= distance && distance < reach[11])) {
        return TRUE;
    }
    return FALSE;
}

/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three melee
   attacks. */
s32 EntityAi_CanMelee(Entity *entity) {
    f32 angle_off[3];
    f32 reach[6]; /* nearest, then farthest, per attack */
    f32 range;
    f32 lower;
    f32 upper;

    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[0] = heading;
            magnitude = angle_off[0];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[0] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[1] = heading;
            magnitude = angle_off[1];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[1] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[2] = heading;
            magnitude = angle_off[2];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[2] = magnitude;
        }
    }
    lower = D_00413F80.position.y - 230.0f;
    upper = D_00413F80.position.y + entity->height;
    range = entity->params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        range *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    reach[3] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06) + entity->radius + 100.0f;
    reach[4] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06) + entity->radius + 100.0f;
    reach[5] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06) + entity->radius + 100.0f;
    if (lower <= entity->model->position[1] && entity->model->position[1] <= upper) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk00 != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk08 && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[3]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk00 != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk08 && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[4]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk00 != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk08 && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[5]) {
            return 1;
        }
    }
    return 0;
}

/* Whether the player is inside the reach (distance, angle and height) of melee record index. */
int EntityAi_CanUseMelee(Entity *entity, int index) {
    EntityParams *params;
    EntityModel *model;
    float angle;
    EntityParamsUnk240 *record;
    float yaw;
    float diff;
    float abs_diff;
    float low;
    float high;
    float reach;
    float window[2];

    params = entity->params;
    model = entity->model;
    record = &ENTITY_PARAMS_UNK240_RECORD(params, index);
    angle = record->unk0E * 0.017453292f;
    if (!(model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = model->unkD0[1] + angle;
    yaw -= (float)(int)(yaw / 6.2831855f) * 6.2831855f;
    if (3.1415927f < yaw) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    diff = entity->unk68 - yaw;
    diff -= (float)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    low = D_00413F80.position.y - 230.0f;
    high = D_00413F80.position.y + entity->height;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).unk06) + entity->radius + 100.0f;
    if (low <= model->position[1] && model->position[1] <= high && abs_diff < ENTITY_PARAMS_UNK240_RECORD(params, index).unk08) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}

/* As EntityAi_CanUseMelee for the entity facing yaw. */
int EntityAi_CanUseMeleeFacing(Entity *entity, int index, float yaw) {
    EntityParams *params;
    EntityModel *model;
    EntityParamsUnk240 *record;
    float angle;
    float diff;
    float abs_diff;
    float low;
    float high;
    float reach;
    float window[2];

    params = entity->params;
    record = &ENTITY_PARAMS_UNK240_RECORD(params, index);
    angle = record->unk0E * 0.017453292f;
    yaw += angle;
    yaw -= (float)(int)(yaw / 6.2831855f) * 6.2831855f;
    if (3.1415927f < yaw) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    diff = entity->unk68 - yaw;
    diff -= (float)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    low = D_00413F80.position.y - 230.0f;
    high = D_00413F80.position.y + entity->height;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).unk06) + entity->radius + 100.0f;
    model = entity->model;
    if (low <= model->position[1] && model->position[1] <= high && abs_diff < ENTITY_PARAMS_UNK240_RECORD(params, index).unk08) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}

/* Whether the player's distance lies in the window of any of the three melee records. */
static int EntityAi_InMeleeDistance(Entity *entity) {
    EntityParams *params;
    float reach;
    float window[6];

    params = entity->params;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    // match: the window bounds re-read entity->params, which keeps a second copy of the pointer
    window[0] = entity->params->unk240[0].unk04 != 0 ? entity->radius + 100.0f + entity->params->unk240[0].unk04 : 0.0f;
    window[1] = entity->params->unk240[1].unk04 != 0 ? entity->radius + 100.0f + entity->params->unk240[1].unk04 : 0.0f;
    window[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    window[3] = (reach < entity->params->unk240[0].unk06 ? reach : entity->params->unk240[0].unk06) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->unk240[1].unk06 ? reach : entity->params->unk240[1].unk06) + entity->radius + 100.0f;
    window[5] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, 2).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, 2).unk06) + entity->radius + 100.0f;
    if ((params->unk240[0].unk00 != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->unk240[1].unk00 != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (ENTITY_PARAMS_UNK240_RECORD(params, 2).unk00 != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
        return 1;
    }
    return 0;
}

/* Whether the player's distance lies in the window of melee record index. */
static int EntityAi_InMeleeDistanceOf(Entity *entity, int index) {
    EntityParams *params;
    float reach;
    float window[2];
    float distance;
    int in_range;

    params = entity->params;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).unk06) + entity->radius + 100.0f;
    distance = entity->distance_to_player;
    in_range = 0;
    if (window[0] <= distance && distance < window[1]) {
        in_range = 1;
    }
    return in_range;
}

/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three ranged
   attacks. */
s32 EntityAi_CanShoot(Entity *entity) {
    f32 angle_off[3];
    f32 reach[6]; /* nearest, then farthest, per attack */
    f32 range;
    f32 top;
    f32 bottom;

    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[0].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[0] = heading;
            magnitude = angle_off[0];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[0] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[1].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[1] = heading;
            magnitude = angle_off[1];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[1] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[2].unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[2] = heading;
            magnitude = angle_off[2];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[2] = magnitude;
        }
    }
    top = entity->model->position[1] - (entity->height + entity->params->unk1A) * 0.5f;
    bottom = entity->model->position[1] - (entity->height - entity->params->unk1A) * 0.5f;
    range = entity->params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        range *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    reach[1] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    reach[2] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    reach[3] = (range < entity->params->unk3D8[0].reach_max ? range : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    reach[4] = (range < entity->params->unk3D8[1].reach_max ? range : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    reach[5] = (range < entity->params->unk3D8[2].reach_max ? range : entity->params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    if (top <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= bottom) {
        if (entity->params->unk3D8[0].record.unk00 != 0 && angle_off[0] < entity->params->unk3D8[0].unk08 && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[3]) {
            return 1;
        }
        if (entity->params->unk3D8[1].record.unk00 != 0 && angle_off[1] < entity->params->unk3D8[1].unk08 && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[4]) {
            return 1;
        }
        if (entity->params->unk3D8[2].record.unk00 != 0 && angle_off[2] < entity->params->unk3D8[2].unk08 && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[5]) {
            return 1;
        }
    }
    return 0;
}

/* Whether the player is inside the reach (distance, angle and height) of ranged record index. */
s32 EntityAi_CanUseRanged(Entity *entity, s32 index) {
    EntityParams *params = entity->params;
    EntityModel *model = entity->model;
    f32 angle;
    f32 yaw;
    f32 diff;
    f32 abs_diff;
    f32 height;
    f32 bottom;
    f32 top;
    f32 farthest;
    f32 reach[2];

    angle = ENTITY_PARAMS_RANGED(params)[index].unk0E * 0.017453292f;
    if (!(model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = wrap_angle(model->unkD0[1] + angle);
    diff = wrap_angle(entity->unk68 - yaw);
    abs_diff = ABS(diff);

    height = entity->height;
    bottom = model->position[1] - (height + params->unk1A) * 0.5f;
    top = model->position[1] - (height - params->unk1A) * 0.5f;

    farthest = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        farthest *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        farthest *= 0.5f;
    }

    reach[0] = ENTITY_PARAMS_RANGED(params)[index].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[index].reach_min : 0.0f;
    reach[1] = (farthest < ENTITY_PARAMS_RANGED(params)[index].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[index].reach_max + entity->radius) + 100.0f;

    if (bottom <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= top && abs_diff < ENTITY_PARAMS_RANGED(params)[index].unk08 &&
        reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[1]) {
        return TRUE;
    }
    return FALSE;
}

/* As EntityAi_CanUseRanged for the entity facing yaw. */
int EntityAi_CanUseRangedFacing(Entity *entity, int index, float yaw) {
    EntityParams *params;
    EntityModel *model;
    float angle;
    float diff;
    float abs_diff;
    float low;
    float high;
    float reach;
    float window[2];

    params = entity->params;
    angle = params->unk3D8[index].unk0E * 0.017453292f;
    yaw += angle;
    yaw -= (float)(int)(yaw / 6.2831855f) * 6.2831855f;
    if (3.1415927f < yaw) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    diff = entity->unk68 - yaw;
    diff -= (float)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    model = entity->model;
    high = model->position[1] - (entity->height + params->unk1A) * 0.5f;
    low = model->position[1] - (entity->height - params->unk1A) * 0.5f;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = params->unk3D8[index].reach_min != 0 ? entity->radius + 100.0f + params->unk3D8[index].reach_min : 0.0f;
    window[1] = (reach < params->unk3D8[index].reach_max ? reach : params->unk3D8[index].reach_max) + entity->radius + 100.0f;
    if (high <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= low && abs_diff < params->unk3D8[index].unk08) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}

/* Whether the player's distance lies in the window of any of the three ranged records. */
static int EntityAi_InRangedDistance(Entity *entity) {
    EntityParams *params;
    float reach;
    float window[6];

    params = entity->params;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    // match: the window bounds re-read entity->params, which keeps a second copy of the pointer
    window[0] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    window[1] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    window[2] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    window[3] = (reach < entity->params->unk3D8[0].reach_max ? reach : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->unk3D8[1].reach_max ? reach : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    window[5] = (reach < params->unk3D8[2].reach_max ? reach : params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    if ((params->unk3D8[0].record.unk00 != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->unk3D8[1].record.unk00 != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (params->unk3D8[2].record.unk00 != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
        return 1;
    }
    return 0;
}

/* Whether the player's distance lies in the window of ranged record index. */
static int EntityAi_InRangedDistanceOf(Entity *entity, int index) {
    EntityParams *params = entity->params;
    f32 reach = params->unk18;
    f32 limits[2];
    f32 near_limit;

    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    if (params->unk3D8[index].reach_min != 0) {
        near_limit = entity->radius + 100.0f + params->unk3D8[index].reach_min;
    } else {
        near_limit = 0.0f;
    }
    limits[0] = near_limit;
    limits[1] = ((reach < params->unk3D8[index].reach_max) ? reach : params->unk3D8[index].reach_max) + entity->radius + 100.0f;
    if (limits[0] <= entity->distance_to_player && entity->distance_to_player < limits[1]) {
        return 1;
    }
    return 0;
}

/* Per-frame collision of an entity: clears its push and state bits, sets bits 22-27 from its params, then
   runs its entity, map object and map piece checks. */
void EntityAi_UpdateCollision(Entity *entity) {
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 51);
    entity->unkA4[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 52);
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 45);
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 46);
    entity->unkA4[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 49);
    entity->unkA4[2] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 48);
    entity->unkB0[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 47);
    entity->unkB0[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 50);
    entity->unkB0[2] = 0;

    if (ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
        return;
    }
    if (!(entity->params->flags & 0x1)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x400000;
    }
    if (!(entity->params->flags & 0x2)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x800000;
    }
    if (!(entity->params->flags & 0x4)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x1000000;
    }
    if (!(entity->params->flags & 0x8)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x2000000;
    }
    if (!(entity->params->flags & 0x10)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x4000000;
    }
    if (!(entity->params->flags & 0x20)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x8000000;
    }

    EntityCollision_PushApart(entity);

    if (!((s32)(ENTITY_FLAG_WORD(entity->flags) >> 25) & 1)) {
        if (ENTITY_PARAMS_WORD(entity->params) & 0x1000000) {
            EntityCollision_SphereAgainstObjects(entity);
        } else {
            EntityCollision_WalkAgainstObjects(entity);
        }
    }
    if (!(ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26)) {
        EntityCollision_WalkAgainstMap(entity);
    } else {
        EntityCollision_SphereAgainstMap(entity);
    }

    if ((u32)(ENTITY_FLAG_WORD(entity->flags) >> 51) & 1) {
        entity->unk48 += 2.0f;
        if (40.0f < entity->unk48) {
            entity->unk48 = 40.0f;
        }
    } else {
        entity->unk48 = 10.0f;
    }
}

/* speed plus EntityParams.unk50, at most EntityParams.unk54. */
float EntityAi_Accelerate(Entity *entity, float speed) {
    EntityParams *params = entity->params;
    float limit = params->unk54;
    float result = speed + params->unk50;

    if (limit < result) {
        result = limit;
    }
    return result;
}

/* speed less EntityParams.unk50, at least 0. */
static float EntityAi_Decelerate(Entity *entity, float speed) {
    float result;

    result = speed - entity->params->unk50;
    if (result < 0.0f) {
        result = 0.0f;
    }
    return result;
}

/* Turns the entity's heading toward target at a rate that builds up to EntityParams.turn_limit; returns 1
   once it is there. */
s32 EntityAi_TurnToHeading(Entity *entity, f32 *speed, f32 target) {
    f32 diff;

    target -= (int)(target / 6.2831855f) * 6.2831855f;
    if (3.1415927f < target) {
        target -= 6.2831855f;
    } else if (target < -3.1415927f) {
        target += 6.2831855f;
    }
    if (entity->model->unkD0[1] == target) {
        *speed = 0.0f;
        return 1;
    }
    diff = target - entity->model->unkD0[1];
    diff -= (int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (diff > 0.0f) {
        *speed += entity->params->unk58;
        if (*speed > entity->params->turn_limit) {
            *speed = entity->params->turn_limit;
        }
        if (diff <= *speed) {
            entity->unk94 += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->unk94 += *speed;
    } else if (diff < 0.0f) {
        *speed -= entity->params->unk58;
        if (*speed < -entity->params->turn_limit) {
            *speed = -entity->params->turn_limit;
        }
        if (*speed <= diff) {
            entity->unk94 += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->unk94 += *speed;
    } else {
        *speed = 0.0f;
        return 1;
    }
    return 0;
}

/* As EntityAi_TurnToHeading, toward a point. */
int EntityAi_TurnToPoint(Entity *entity, f32 *turn_rate, f32 *target) {
    f32 angle;
    f32 diff;

    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->model->unkD0[1] == angle) {
        *turn_rate = 0.0f;
        return 1;
    }
    diff = angle - entity->model->unkD0[1];
    diff -= (f32)(int)(diff / 6.2831855f) * 6.2831855f;
    if (diff > 3.1415927f) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (diff > 0.0f) {
        *turn_rate += entity->params->unk58;
        if (*turn_rate > entity->params->turn_limit) {
            *turn_rate = entity->params->turn_limit;
        }
        if (diff <= *turn_rate) {
            entity->unk94 += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->unk94 += *turn_rate;
    } else if (diff < 0.0f) {
        *turn_rate -= entity->params->unk58;
        if (*turn_rate < -entity->params->turn_limit) {
            *turn_rate = -entity->params->turn_limit;
        }
        if (*turn_rate <= diff) {
            entity->unk94 += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->unk94 += *turn_rate;
    } else {
        *turn_rate = 0.0f;
        return 1;
    }
    return 0;
}

/* Sets the pause before the next attack (Entity.unkA2): base plus a share of the player's health in percent,
   more for each kind of healing item he carries. */
void EntityAi_SetAttackDelay(Entity *entity, s32 base) {
    s32 delay;

    delay = (D_00413F80.hp_now * 100) / D_00413F80.hp_max;
    delay = (item_counts[ITEM_HERB] == 0) ? delay : delay + 5;
    delay = (item_counts[ITEM_HERBAL_LIQUID] != 0) ? delay + 0xF : delay;
    delay = (item_counts[ITEM_FOREST_FRUIT] != 0) ? delay + 0x1E : delay;
    delay = (item_counts[ITEM_DRAGON_FLOWER] != 0) ? delay + 0x32 : delay;
    entity->unkA2 = base + (delay * 0x1E) / 200;
}

/* Sets the entity's reaction, the handler that runs in place of its action until it clears itself (NULL
   clears it), and puts the reaction's step, Entity.unkCB, back to 0; optionally runs it now. */
int EntityAi_SetReaction(Entity *entity, void (*reaction)(void), int run_now) {
    entity->unkE0.reaction = reaction;
    entity->unkCB = 0;
    if (run_now != 0) {
        if (reaction != NULL) {
            reaction();
        }
    }
    return 1;
}

/* Raises the weight factor (Entity.unkC0) of the attack in flag bits 58-59 by 5, kept within 0x40..0xC0. */
void EntityAi_RaiseAttackWeight(Entity *entity) {
    u8 *level = entity->unkC0;
    s32 value;
    s32 limit = 0xC0; // match: the 0xC0 cap stays in a register, as in EntityAi_LowerWeightsAfterHit

    level += ENTITY_FLAG_BITS(entity)->bits58;
    value = *level + 5;
    if (value < 0x40) {
        value = 0x40;
    }
    if (value > limit) {
        value = limit;
    }
    *level = value;
}

/* Raises the weight factors of the three ranged attacks (Entity.unkC0[3..5]) by 2, kept within 0x40..0xC0. */
void EntityAi_RaiseRangedWeights(Entity *entity) {
    adjust_level(&entity->unkC0[3], 2, 0xC0);
    adjust_level(&entity->unkC0[4], 2, 0xC0);
    adjust_level(&entity->unkC0[5], 2, 0xC0);
}

/* func_0019A418 for the motion of animation index (EntityParams.unk60). */
int func_0019A3F0(Entity *entity, s32 index) {
    return func_0019A418(ENTITY_MODEL(entity), entity->params->unk60[index]);
}

/* The u16 in the four bytes in front of the block ModelInstance.level_tables[index] points at. */
u16 func_0019A418(ModelInstance *model, int index) {
    ModelPartLevels *levels;

    /* match: indexing level_tables as an array swaps the addu operands */
    levels = *(ModelPartLevels **)((char *)model->level_tables + (index << 2));
    return *(u16 *)((char *)levels - 4);
}

/* Sets byte unk6C of model part index. */
void func_0019A430(ModelInstance *model, int index, u8 value) {
    model->parts[index].unk6C = value;
}

/* Starts animation in a motion slot of the work area on channel, from frame, over frames (a quarter longer
   while Entity.unk28 runs and it is not busy); 1 without a change when the slot already plays it and force
   is 0. */
int EntityAi_StartSlotMotion(Entity *entity, EntityMotionSlotWord *slot, int channel, int animation, int frame, int frames, int force) {
    u8 *table = entity->params->unk60;
    int wanted = table[animation];
    int current = table[*(u8 *)slot & 0x1F];

    if (force == 0 && current == wanted) {
        return 1;
    }
    slot->frame_fixed = frame << 8;
    slot->index = animation;
    slot->frame = frame;
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        float slowed = frames;

        slot->unk01 = slowed + slowed * 0.75f;
    } else {
        slot->unk01 = frames;
    }
    if (*(int *)slot & 0x20) {
        ModelPart *root = ENTITY_MODEL(entity)->unk130;

        root->unk40[0] = root->unk40[1] = root->unk40[2] = 0.0f;
    }
    return Motion_SeekSlotKeys(ENTITY_MODEL(entity), channel, wanted, frame);
}

/* Plays a motion slot a step on (Motion_BlendSlotToPose with weight) and returns its frames left, counted down by
   one. */
int EntityAi_StepSlotMotion(Entity *entity, EntityMotionSlot *slot, int channel, f32 weight) {
    u8 frames_left;

    Motion_BlendSlotToPose(ENTITY_MODEL(entity), channel, entity->params->unk60[slot->index], slot->frame, weight);
    slot->unk01 = frames_left = slot->unk01 - 1;
    return frames_left;
}

/* Cubic Hermite interpolation: weights the two points and two tangents by the Hermite basis at ratio and
   hands them to Vector_WeightedSum4. */
void Vector_Hermite(void *result, void *point0, void *point1, void *tangent0, void *tangent1, f32 ratio) {
    f32 basis[4];
    f32 squared = ratio * ratio;
    f32 cubed = squared * ratio;

    basis[0] = cubed + cubed - 3.0f * squared + 1.0f;
    basis[1] = cubed * -2.0f + 3.0f * squared;
    basis[2] = cubed - 2.0f * squared + ratio;
    basis[3] = cubed - squared;
    Vector_WeightedSum4(result, point0, point1, tangent0, tangent1, basis);
}

/* Draws, for each live entity with flag bit 17, its pool block's model again with the entity's pose and
   alpha. */
void Entity_DrawPoolModels(void) {
    Entity *entity;
    ModelInstance *model;
    ModelPart *dst;
    ModelPart *src;
    s32 i;
    s32 count;

    func_0010A718(0);
    func_00109A50();
    func_00109A80(0x1A, 0);
    func_00109A80(0x1B, 0x248);
    func_00109A80(GS_REG_TEST_2, 0x55001);
    func_00109A80(0x4B, 1);
    func_00109AD0();
    func_00109E18();
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (!ENTITY_FLAG_BITS(entity)->bit1 || entity->unk14 == NULL || (f32)entity->params->unk04 <= 0.0f) {
            continue;
        }
        if (!ENTITY_FLAG_BITS(entity)->bit17) {
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->bit30) {
            continue;
        }
        model = &D_003BE530[entity->unkD3];
        if (model->parts == NULL) {
            continue;
        }
        if (ENTITY_MODEL(entity)->part_count != model->part_count) {
            continue;
        }
        // match: each part's matrix is moved as four 128-bit rows (lq/sq)
        dst = model->parts;
        src = ENTITY_MODEL(entity)->parts;
        count = model->part_count;
        for (i = 0; i < count; i++) {
            ModelMatrixQwords *dst_matrix = (ModelMatrixQwords *)dst->unk00;
            ModelMatrixQwords *src_matrix = (ModelMatrixQwords *)src->unk00;

            dst_matrix->rows[0] = src_matrix->rows[0];
            dst_matrix->rows[1] = src_matrix->rows[1];
            dst_matrix->rows[2] = src_matrix->rows[2];
            dst_matrix->rows[3] = src_matrix->rows[3];
            dst++;
            src++;
        }
        Matrix_Multiply(model->unk40, ENTITY_MODEL(entity)->unk40, D_003E74C0);
        model->unk148 = ENTITY_MODEL(entity)->unk148;
        model->unk14C = ENTITY_MODEL(entity)->unk14C;
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_2, ((s64)(s32)(ENTITY_MODEL(entity)->alpha * 32.0f * 0.0078125f) << 32) | 0x64);
        func_00109AD0();
        model->unk1A8(model);
        func_00109E18();
    }
    func_00109A50();
    func_00109A80(0x1A, 1);
    func_00109A80(0x4B, 0);
    func_00109AD0();
}
