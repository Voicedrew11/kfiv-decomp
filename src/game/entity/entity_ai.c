/* The shared entity AI helpers: handler switching, attack reach tests, turning, the attack
   delay, the motion slots of the work area. */
#include "common.h"
#include "entity/entity.h"
#include "effect/shot.h"
#include "effect/effect.h"
#include "player/player.h"
#include "map/map.h"
#include "model/model.h"
#include "entity/entity_ai.h"

#include "gfx/gs.h"
#include "core/file.h"
#include "core/game.h"
#include "save/save_state.h"
#include "enemy/enemy.h"
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include "entity/entity_events.h"
#include "gfx/gs_draw.h"
#include "gfx/packet.h"
#include <math.h>
#include "math/camera.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>

/* EntityMath_WrapAngle, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}
/* match: the call leaves the sixth argument (rate, $f12) unset */
#define TURN_PART_NO_RATE ((int (*)(void *, f32 *, int, f32 *, f32 *))Bone_TurnPart)
#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->spawn_flags)
// match: reading EntityParams word 0 whole; a bit-field in byte 3 would be an lbu
typedef union {
    u32 word;
} EntityParamsFirstWord;
#define ENTITY_PARAMS_WORD(params) (((EntityParamsFirstWord *)(params))->word)
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

static s32 EntityAi_CheckMeleeAilment(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame_min, s32 frame_max);
static void EntityAi_Kill(Entity *entity);
static int EntityAi_TurnLookPart(Entity *actor, f32 *out_a, int mode, f32 *out_b, f32 *limit);
static int EntityAi_TurnAimPart(Entity *entity, f32 *angles, int mode, f32 *goal, f32 *limits);
static int EntityAi_LookAtAimPart(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale);
static int EntityAi_LookAtAimPartFromPlayerHead(Entity *entity, int angles, int mode, int limits, float rate);
static int EntityAi_IsPlayerNear(Entity *entity);
static void EntityAi_SensePlayerInArc(Entity *actor);
static void EntityAi_RollNoticePlayer(Entity *actor);
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

    angle = DEG_TO_RAD((float) (rand() % 360));
    if (PI <= angle) {
        angle -= TWO_PI;
    }
    return angle;
}

/* Starts the second camera shake (GameLoop_UpdateCameraShake plays it): its reach (CameraState.entity_shake_reach) and its frames
   (unk7C). The Giant Beetle's ranged attack uses it. */
void EntityAi_ShakeCamera(int float_value, int int_value) {
    camera_state.entity_shake_reach = float_value;
    camera_state.entity_shake_frames = int_value;
}

/* Sets Entity.state and keeps the old value in prev_state. */
void EntityAi_SetState(Entity *entity, u8 state) {
    u8 old_state;

    old_state = entity->state;
    entity->state = state;
    entity->prev_state = old_state;
}

/* Installs the entity's per-frame update (Entity.update_func; the type table entity_type_handlers gives it) and puts
   action_step back to 0. */
void EntityAi_SetUpdate(Entity *entity, EntityHandler update) {
    entity->update_func = update;
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
    ENTITY_FLAG_BITS(entity)->ranged_attack = bit57;
    ENTITY_FLAG_BITS(entity)->attack_index = bits58;
    entity->action_step = 0;
    entity->prev_handler = entity->handler;
    entity->handler = handler;
    if (run != 0 && handler != NULL) {
        handler(entity);
    }
    return 1;
}

/* As EntityAi_CheckMeleeHit, but hands the part to EntityAim_AilPlayerInArc, which tests the player alone (heading
   turned round by pi) and gives him the part's ailments; returns its result. Nothing calls it. */
static s32 EntityAi_CheckMeleeAilment(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame_min, s32 frame_max) {
    EntityModel *model;
    f32 turn;
    f32 wrapped;

    model = entity->model;
    turn = DEG_TO_RAD(record->heading_offset_deg);
    if (!(0.0f <= model->scale[0])) {
        turn = -turn;
    }
    wrapped = model->rotation[1] + turn + PI;
    wrapped = wrapped - (f32)(s32)(wrapped / TWO_PI) * TWO_PI;
    if (PI < wrapped) {
        wrapped -= TWO_PI;
    } else if (wrapped < -PI) {
        wrapped += TWO_PI;
    }
    if ((frame_min < record->hit_parts[index].first_frame && frame_max < record->hit_parts[index].first_frame) || (record->hit_parts[index].last_frame < frame_min && record->hit_parts[index].last_frame < frame_max)) {
        return 0;
    }
    return EntityAim_AilPlayerInArc(&record->hit_parts[index].ailment_chances, model->position, DEG_TO_RAD(record->hit_parts[index].half_angle_deg), entity->collision_height, record->hit_parts[index].radius_bonus, wrapped);
}

/* Hit check of one part of a melee record: turns the model's heading by the record's angle (mirrored when
   the model is) and hands it to EntityAi_CheckMeleeHitAt. */
int EntityAi_CheckMeleeHit(Entity *entity, EntityParamsUnk240 *record, int part, int frame_from, int frame_to) {
    ModelInstance *model = ENTITY_MODEL(entity);
    float delta = DEG_TO_RAD((float)record->heading_offset_deg);
    float angle;

    if (!(model->scale_x >= 0.0f)) {
        delta = -delta;
    }
    angle = model->rotation.y + delta;
    angle = angle - (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return EntityAi_CheckMeleeHitAt(entity, record, part, frame_from, frame_to, angle);
}

/* Runs EntityAi_SendMeleeHit for one part of a melee record facing angle, unless the animation frames
   [frame_from, frame_to] lie outside the frames the part is active on (0 then). */
s32 EntityAi_CheckMeleeHitAt(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
    if (unk240->hit_parts[part].first_frame == 0 ||
        (frame_from < unk240->hit_parts[part].first_frame && frame_to < unk240->hit_parts[part].first_frame) ||
        (unk240->hit_parts[part].last_frame < frame_from && unk240->hit_parts[part].last_frame < frame_to)) {
        return 0;
    }
    return EntityAi_SendMeleeHit(entity, unk240, part, angle);
}

/* Builds the attack request of one part of a melee record (reach, height, angle, power) and runs it with
   Attack_Run; returns its result. */
s32 EntityAi_SendMeleeHit(Entity *entity, EntityParamsUnk240 *record, s32 index, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;

    memset(&request, 0, sizeof(request));
    memset(&extra, 0, sizeof(extra));
    Vector_Copy(request.position, entity->model->position);
    yaw = angle + PI;
    request.radius = entity->radius + record->hit_parts[index].radius_bonus;
    request.height = entity->height + 10.0f;
    request.facing = wrap_angle(yaw);
    request.half_angle = wrap_angle(DEG_TO_RAD(record->hit_parts[index].half_angle_deg));
    request.owner = entity;
    request.shape = 3;
    request.target_mask = 1;
    request.strengths = record->hit_parts[index].strengths;
    request.ailment_chances = record->hit_parts[index].ailment_chances;
    request.hit_armor_part = record->wear_armour_part;
    request.knockback = record->hit_parts[index].hit_flags & 1;
    if ((ENTITY_PART_FLAGS(&record->hit_parts[index])->flags >> 1) & 1) {
        request.mp_damage = record->hit_parts[index].mp_damage;
    }
    extra.is_magic_shot = 0;
    extra.strength_gauge = 0;
    extra.player_power = 0;
    extra.player_magic = 0;
    return Attack_Run(&request, &extra);
}

/* Allocates a shot carrying EntityParams.unk238 and launches effect effect_id from origin along angles
   (Attack_LaunchMagicEffect) for the entity. */
int EntityAi_LaunchShot(Entity *entity, int effect_id, void *origin, void *angles) {
    EntityUnk1B1390 *shot;

    shot = Shot_Alloc();
    shot->player_magic = shot->player_power = 0;
    shot->is_magic_shot = 1;
    shot->shot_params = entity->params->shot_params;
    return Attack_LaunchMagicEffect((u16)effect_id, shot, origin, angles, entity);
}

/* EntityShot_LaunchAimed with no target: launches the shots of a ranged record whose frame lies in (frame_from,
   frame_to]. */
int EntityAi_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 elapsed, s32 unk82) {
    return EntityShot_LaunchAimed(entity, unk3D8, elapsed, unk82, NULL, 0.0f, 0.0f);
}

/* Spawns an effect at a model point, facing along the line to the next point. */
void EntityAi_SpawnBoneEffect(Entity *entity, int effect_id, int point) {
    f32 *from;
    f32 *tip;
    f32 angles[4];
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 yaw;
    f32 turns;

    from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, point);
    tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, point + 1);
    if (from != NULL && tip != NULL) {
        delta_x = tip[0] - from[0];
        delta_y = tip[1] - from[1];
        delta_z = tip[2] - from[2];
        angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
        yaw = -atan2f(delta_z, delta_x) + HALF_PI;
        turns = (int)(yaw / TWO_PI);
        yaw -= turns * TWO_PI;
        if (PI < yaw) {
            yaw -= TWO_PI;
        } else if (yaw < -PI) {
            yaw += TWO_PI;
        }
        angles[1] = yaw;
        angles[2] = angles[3] = 0.0f;
        Effect_StartSpawner(effect_id, from, angles, entity);
    }
}

/* Sets Entity.hit_points (its health) to 0, so EntityAi_Update starts the death action. Nothing calls it by name. */
static void EntityAi_Kill(Entity *entity) {
    entity->hit_points = 0;
}

/* Bone_TurnPart for the model part EntityAi_LookAtPoint turns (EntityModel.look_at_part, the third part the model
   file marks) while params flag 0x2000 is set; FALSE without it. Nothing calls it. */
static int EntityAi_TurnLookPart(Entity *actor, f32 *out_a, int mode, f32 *out_b, f32 *limit) {
    void *target = actor->model->look_at_part;

    if (target == NULL) {
        return 0;
    }
    if (!(ENTITY_CONTROLLER(actor)->flags & ENTITY_PARAMS_FLAG_LOOK_AT_PART_TURNS)) {
        return 0;
    }
    return TURN_PART_NO_RATE(target, out_a, mode, out_b, limit);
}

/* Turns the model part at EntityModel.look_at_part to look at target (Bone_TurnPartToLook) while params flag 0x2000 is
   set; FALSE without it. */
int EntityAi_LookAtPoint(Entity *entity, int angles, int mode, int target, int limits, float rate) {
    EntityModel *model = entity->model;
    int part = (int)model->look_at_part;

    if (part == 0 || !(ENTITY_CONTROLLER(entity)->flags & ENTITY_PARAMS_FLAG_LOOK_AT_PART_TURNS)) {
        return 0;
    }
    return Bone_TurnPartToLook(model, part, angles, mode, (void *)target, limits, rate);
}

/* As EntityAi_LookAtPoint, at the player's position 220 above his feet. */
int EntityAi_LookAtPlayer(Entity *entity, void *target, s32 mode, void *out, f32 range) {
    f32 position[4];
    int result;

    Vector_Copy(position, &player_state.position);
    position[1] -= 220.0f;
    if (entity->model->look_at_part == NULL || !(entity->params->flags & ENTITY_PARAMS_FLAG_LOOK_AT_PART_TURNS)) {
        result = 0;
    } else {
        result = Bone_TurnPartToLook(entity->model, (int)entity->model->look_at_part, (int)target, mode, position, (int)out, range);
    }
    return result;
}

/* Bone_TurnPart for the model part at EntityModel.aim_part (the one the model file marks with ModelFilePart.unk06
   2); FALSE without it. Nothing calls it, and no code that runs uses that part, so its role is not known. */
static int EntityAi_TurnAimPart(Entity *entity, f32 *angles, int mode, f32 *goal, f32 *limits) {
    int part = entity->model->aim_part;
    int result;

    if (part == 0) {
        result = 0;
    } else {
        result = TURN_PART_NO_RATE((void *)part, angles, mode, goal, limits);
    }
    return result;
}

/* As EntityAi_LookAtPoint, for the model part at EntityModel.aim_part (role unknown, see EntityAi_TurnAimPart) and
   without the params flag test; FALSE without the part. Nothing calls it. */
static int EntityAi_LookAtAimPart(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale) {
    EntityModel *model = entity->model;

    if (model->aim_part == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->aim_part, value_a, value_b, value_c, value_d, scale);
}

/* As EntityAi_LookAtAimPart, at the player's position 115 above his feet; FALSE without the part.
   Nothing calls it. */
static int EntityAi_LookAtAimPartFromPlayerHead(Entity *entity, int angles, int mode, int limits, float rate) {
    float position[2]; // match: Vector_Copy writes four floats, but retail reserves room for two
    EntityModel *model;

    Vector_Copy(position, &player_state.position);
    position[1] -= 115.0f;
    model = entity->model;
    if (model->aim_part == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->aim_part, angles, mode, position, limits, rate);
}

/* Once the model is below y 10000 (fallen out of the map): frees its effects, clears flag bit 1 and returns
   1; else 0. */
s32 EntityAi_RemoveIfFallen(Entity *entity) {
    u16 ctrl_flags;

    if (*(f32 *)((char *)entity->model + 0xC4) > 10000.0f) {
        ctrl_flags = ENTITY_CONTROLLER(entity)->respawn_effect_id;
        if (ctrl_flags != 0 && ctrl_flags != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->flags &= ~ENTITY_FLAG_LIVE;
        return 1;
    }
    return 0;
}

/* 1 when the player is within 8000, unless the entity stands in a map record (Entity.map_record) whose flags lack
   0x04000000 (0 then). Nothing calls it. */
static int EntityAi_IsPlayerNear(Entity *entity) {
    if (entity->map_record != NULL && (((EntitySpawnInfo *)entity->map_record)->spawn_flags & 0x04000000) == 0) {
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

    if (!ENTITY_FLAG_BITS(entity)->respawnable) {
        return FALSE;
    }
    if (ENTITY_FLAG_BITS(entity)->no_respawn && (entity->spawn_info->spawn_flags & (0x8000L << 46))) {
        return FALSE;
    }
    death_count = Game_GetSpawnDeathCount(map_slots[entity->group_id].tag, entity->spawn_info - map_spawn_infos[entity->group_id]);
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
        : "=m"(distance) : "r"(&spawn_info->spawn_x), "r"(&player_state.position), "r"(distance));
    if (distance[0] <= (f32)spawn_info->respawn_distance) {
        if ((entity->spawn_info->spawn_flags & (0xC000L << 45)) == (0x8000L << 44)) {
            return TRUE;
        }
        if (ENTITY_FLAG_BITS(entity)->respawn_roll_done) {
            return FALSE;
        }
        ENTITY_FLAG_BITS(entity)->respawn_roll_done = TRUE;
        if (rand() % 100 < entity->spawn_info->respawn_chance) {
            return TRUE;
        }
    } else {
        ENTITY_FLAG_BITS(entity)->respawn_roll_done = FALSE;
    }
    return FALSE;
}

/* The flag-32 half of EntityAi_SensePlayer on its own: sets flag 32 while the player is within reach, arc
   and height. Nothing calls it. */
static void EntityAi_SensePlayerInArc(Entity *actor) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 reach = ctrl->attack_range;
    f32 half_height = ctrl->attack_height_band;
    f32 arc = ctrl->attack_arc;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;

    if (actor->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && actor->distance_to_player_3d < player_state.intimidate_range)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    turn = actor->heading_to_player - actor->model->rotation[1];
    turn -= (s32)(turn / TWO_PI) * TWO_PI;
    if (PI < turn) {
        turn -= TWO_PI;
    } else if (turn < -PI) {
        turn += TWO_PI;
    }
    turn_abs = 0.0f <= turn ? turn : -turn;
    upper = actor->model->position[1] - (actor->height + half_height) * 0.5f;
    lower = actor->model->position[1] - (actor->height - half_height) * 0.5f;
    ENTITY_FLAG_BITS(actor)->player_in_reach = 0;
    if (actor->distance_to_player < actor->radius + 100.0f + reach && turn_abs < arc && upper <= player_state.position.y && player_state.position.y - 230.0f <= lower) {
        ENTITY_FLAG_BITS(actor)->player_in_reach = 1;
    }
}

/* The flag-33 half of EntityAi_SensePlayer on its own: sets flag 33 (noticed) by a chance that grows as the
   player comes nearer, clears it when he is out of reach. Nothing calls it. */
static void EntityAi_RollNoticePlayer(Entity *actor) {
    f32 reach = ENTITY_CONTROLLER(actor)->attack_range;
    f32 limit;

    if (actor->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && actor->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + reach;
    if (actor->flags & (ENTITY_FLAG_PLAYER_IN_REACH | ENTITY_FLAG_PLAYER_PUSH)) {
        actor->flags |= ENTITY_FLAG_PLAYER_NOTICED;
    } else if (limit < actor->distance_to_player) {
        actor->flags &= ~ENTITY_FLAG_PLAYER_NOTICED;
    } else {
        s32 chance = (s32)((limit - actor->distance_to_player) * 100.0f / limit) * 2;

        if (rand() % 100 < chance) {
            actor->flags |= ENTITY_FLAG_PLAYER_NOTICED;
        }
    }
}

/* Sets flag 32 while the player is within reach, arc and height, and flag 33 (noticed) by a chance that
   grows as he comes nearer; flag 63 when something stands between them, which clears flag 32 (mode below 2)
   and flag 33 (mode 0). */
void EntityAi_SensePlayer(Entity *actor, s32 mode) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 half_height = ctrl->attack_height_band;
    f32 reach = ctrl->attack_range;
    f32 arc = ctrl->attack_arc;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;
    f32 near_reach;
    f32 limit;

    if (actor->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && actor->distance_to_player_3d < player_state.intimidate_range)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    turn = actor->heading_to_player - actor->model->rotation[1];
    turn -= (s32)(turn / TWO_PI) * TWO_PI;
    if (PI < turn) {
        turn -= TWO_PI;
    } else if (turn < -PI) {
        turn += TWO_PI;
    }
    turn_abs = 0.0f <= turn ? turn : -turn;
    upper = actor->model->position[1] - (actor->height + half_height) * 0.5f;
    lower = actor->model->position[1] - (actor->height - half_height) * 0.5f;
    ENTITY_FLAG_BITS(actor)->player_in_reach = 0;
    if (actor->distance_to_player < actor->radius + 100.0f + reach && turn_abs < arc && upper <= player_state.position.y && player_state.position.y - 230.0f <= lower) {
        ENTITY_FLAG_BITS(actor)->player_in_reach = 1;
    }
    near_reach = ctrl->attack_range;
    if (actor->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && actor->distance_to_player_3d < player_state.intimidate_range)) {
        near_reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        near_reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + near_reach;
    if (actor->flags & (ENTITY_FLAG_PLAYER_IN_REACH | ENTITY_FLAG_PLAYER_PUSH)) {
        actor->flags |= ENTITY_FLAG_PLAYER_NOTICED;
    } else if (limit < actor->distance_to_player) {
        actor->flags &= ~ENTITY_FLAG_PLAYER_NOTICED;
    } else {
        s32 chance = (s32)((limit - actor->distance_to_player) * 100.0f / limit) * 2;

        if (rand() % 100 < chance) {
            actor->flags |= ENTITY_FLAG_PLAYER_NOTICED;
        }
    }
    actor->flags &= ~ENTITY_FLAG_SIGHT_BLOCKED;
    if (EntitySight_PropInWay(actor) - 1 < 3U || EntitySight_ObjectInWay(actor) != 0) {
        actor->flags |= ENTITY_FLAG_SIGHT_BLOCKED;
        if (mode < 2) {
            actor->flags &= ~ENTITY_FLAG_PLAYER_IN_REACH;
            if (mode == 0) {
                actor->flags &= ~ENTITY_FLAG_PLAYER_NOTICED;
            }
        }
    }
}

/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.wander_radius (in x, y and z),
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
    limit = spawn_info->wander_radius;
    delta_z = model->position[2] - spawn_info->spawn_z;
    if ((float)(limit * limit) < (delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z)) {
        entity->flags |= ENTITY_FLAG_FAR_FROM_SPAWN;
        return;
    }
    entity->flags &= ~ENTITY_FLAG_FAR_FROM_SPAWN;
}

/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.wander_radius (in x and z),
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
    limit = spawn_info->wander_radius;
    delta_z = model->position[2] - spawn_info->spawn_z;
    if ((float)(limit * limit) < (delta_x * delta_x) + (delta_z * delta_z)) {
        entity->flags |= ENTITY_FLAG_FAR_FROM_SPAWN;
        return;
    }
    entity->flags &= ~ENTITY_FLAG_FAR_FROM_SPAWN;
}

/* Clears flag bit 31. The think functions call it when closing in and test the bit right after: set, they
   would only turn on the spot to the player. Nothing in the game sets it, so they always walk. */
void EntityAi_ClearTurnInPlace(Entity *entity) {
    entity->flags &= ~ENTITY_FLAG_TURN_IN_PLACE;
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
        angle += PI;
        angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
        if (PI < angle) {
            angle -= TWO_PI;
        } else if (angle < -PI) {
            angle += TWO_PI;
        }
        angle -= entity->model->rotation[1];
        angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
        if (PI < angle) {
            angle -= TWO_PI;
        } else if (angle < -PI) {
            angle += TWO_PI;
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
        diff -= entity->model->rotation[0];
        diff = diff - (f32)(s32)(diff / TWO_PI) * TWO_PI;
        if (PI < diff) {
            diff -= TWO_PI;
        } else if (diff < -PI) {
            diff += TWO_PI;
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

    if (params->melee_attacks[0].attack_weight != 0 || params->melee_attacks[1].attack_weight != 0 || (u8)params->melee_attack_third[0] != 0) {
        return 1;
    }
    return 0;
}

/* Nonzero when any of the three ranged records has a weight. */
static int EntityAi_HasRangedAttack(Entity *entity) {
    EntityParams *params = entity->params;

    if (params->ranged_attacks[0].record.ranged_weight != 0 || ENTITY_PARAMS_UNK3D8(params, 1)->ranged_weight != 0 || ENTITY_PARAMS_UNK3D8(params, 2)->ranged_weight != 0) {
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[0].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[1].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[2].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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
    top = entity->model->position[1] - (entity->height + entity->params->attack_height_band) * 0.5f;
    bottom = entity->model->position[1] - (entity->height - entity->params->attack_height_band) * 0.5f;
    range = entity->params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        range *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min : 0.0f;
    reach[3] = entity->params->ranged_attacks[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[0].reach_min : 0.0f;
    reach[4] = entity->params->ranged_attacks[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[1].reach_min : 0.0f;
    reach[5] = entity->params->ranged_attacks[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[2].reach_min : 0.0f;
    reach[6] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max) + entity->radius + 100.0f;
    reach[7] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max) + entity->radius + 100.0f;
    reach[8] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max) + entity->radius + 100.0f;
    reach[9] = (range < entity->params->ranged_attacks[0].reach_max ? range : entity->params->ranged_attacks[0].reach_max) + entity->radius + 100.0f;
    reach[10] = (range < entity->params->ranged_attacks[1].reach_max ? range : entity->params->ranged_attacks[1].reach_max) + entity->radius + 100.0f;
    reach[11] = (range < entity->params->ranged_attacks[2].reach_max ? range : entity->params->ranged_attacks[2].reach_max) + entity->radius + 100.0f;
    if (entity->model->position[1] - entity->height <= player_state.position.y && player_state.position.y - 230.0f <= entity->model->position[1]) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).attack_weight != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_angle && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[6]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).attack_weight != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_angle && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[7]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).attack_weight != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_angle && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[8]) {
            return 1;
        }
    }
    if (top <= player_state.position.y && player_state.position.y - 230.0f <= bottom) {
        if (entity->params->ranged_attacks[0].record.ranged_weight != 0 && angle_off[3] < entity->params->ranged_attacks[0].widest_angle && reach[3] <= entity->distance_to_player && entity->distance_to_player < reach[9]) {
            return 1;
        }
        if (entity->params->ranged_attacks[1].record.ranged_weight != 0 && angle_off[4] < entity->params->ranged_attacks[1].widest_angle && reach[4] <= entity->distance_to_player && entity->distance_to_player < reach[10]) {
            return 1;
        }
        if (entity->params->ranged_attacks[2].record.ranged_weight != 0 && angle_off[5] < entity->params->ranged_attacks[2].widest_angle && reach[5] <= entity->distance_to_player && entity->distance_to_player < reach[11]) {
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

    range = entity->params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        range *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min : 0.0f;
    reach[3] = entity->params->ranged_attacks[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[0].reach_min : 0.0f;
    reach[4] = entity->params->ranged_attacks[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[1].reach_min : 0.0f;
    reach[5] = entity->params->ranged_attacks[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[2].reach_min : 0.0f;
    reach[6] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max) + entity->radius + 100.0f;
    reach[7] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max) + entity->radius + 100.0f;
    reach[8] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max) + entity->radius + 100.0f;
    reach[9] = (range < entity->params->ranged_attacks[0].reach_max ? range : entity->params->ranged_attacks[0].reach_max) + entity->radius + 100.0f;
    reach[10] = (range < entity->params->ranged_attacks[1].reach_max ? range : entity->params->ranged_attacks[1].reach_max) + entity->radius + 100.0f;
    reach[11] = (range < entity->params->ranged_attacks[2].reach_max ? range : entity->params->ranged_attacks[2].reach_max) + entity->radius + 100.0f;
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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
    lower = player_state.position.y - 230.0f;
    upper = player_state.position.y + entity->height;
    range = entity->params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        range *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_min : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_min : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min : 0.0f;
    reach[3] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_max) + entity->radius + 100.0f;
    reach[4] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_max) + entity->radius + 100.0f;
    reach[5] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_max) + entity->radius + 100.0f;
    if (lower <= entity->model->position[1] && entity->model->position[1] <= upper) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).attack_weight != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).reach_angle && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[3]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).attack_weight != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).reach_angle && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[4]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).attack_weight != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_angle && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[5]) {
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
    angle = DEG_TO_RAD(record->heading_offset_deg);
    if (!(model->scale[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = model->rotation[1] + angle;
    yaw -= (float)(int)(yaw / TWO_PI) * TWO_PI;
    if (PI < yaw) {
        yaw -= TWO_PI;
    } else if (yaw < -PI) {
        yaw += TWO_PI;
    }
    diff = entity->heading_to_player - yaw;
    diff -= (float)(int)(diff / TWO_PI) * TWO_PI;
    if (PI < diff) {
        diff -= TWO_PI;
    } else if (diff < -PI) {
        diff += TWO_PI;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    low = player_state.position.y - 230.0f;
    high = player_state.position.y + entity->height;
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max) + entity->radius + 100.0f;
    if (low <= model->position[1] && model->position[1] <= high && abs_diff < ENTITY_PARAMS_UNK240_RECORD(params, index).reach_angle) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}

/* As EntityAi_CanUseMelee for the entity facing yaw. */
int EntityAi_CanUseMeleeFacing(Entity *entity, float yaw, int index) {
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
    angle = DEG_TO_RAD(record->heading_offset_deg);
    yaw += angle;
    yaw -= (float)(int)(yaw / TWO_PI) * TWO_PI;
    if (PI < yaw) {
        yaw -= TWO_PI;
    } else if (yaw < -PI) {
        yaw += TWO_PI;
    }
    diff = entity->heading_to_player - yaw;
    diff -= (float)(int)(diff / TWO_PI) * TWO_PI;
    if (PI < diff) {
        diff -= TWO_PI;
    } else if (diff < -PI) {
        diff += TWO_PI;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    low = player_state.position.y - 230.0f;
    high = player_state.position.y + entity->height;
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max) + entity->radius + 100.0f;
    model = entity->model;
    if (low <= model->position[1] && model->position[1] <= high && abs_diff < ENTITY_PARAMS_UNK240_RECORD(params, index).reach_angle) {
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
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    // match: the window bounds re-read entity->params, which keeps a second copy of the pointer
    window[0] = entity->params->melee_attacks[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->melee_attacks[0].reach_min : 0.0f;
    window[1] = entity->params->melee_attacks[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->melee_attacks[1].reach_min : 0.0f;
    window[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).reach_min : 0.0f;
    window[3] = (reach < entity->params->melee_attacks[0].reach_max ? reach : entity->params->melee_attacks[0].reach_max) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->melee_attacks[1].reach_max ? reach : entity->params->melee_attacks[1].reach_max) + entity->radius + 100.0f;
    window[5] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, 2).reach_max ? reach : ENTITY_PARAMS_UNK240_RECORD(params, 2).reach_max) + entity->radius + 100.0f;
    if ((params->melee_attacks[0].attack_weight != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->melee_attacks[1].attack_weight != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (ENTITY_PARAMS_UNK240_RECORD(params, 2).attack_weight != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
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
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).reach_min : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).reach_max) + entity->radius + 100.0f;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[0].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[1].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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

        turn = DEG_TO_RAD(entity->params->ranged_attacks[2].heading_offset_deg);
        if (!(entity->model->scale[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->rotation[1] + turn;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
        }
        heading = entity->heading_to_player - heading;
        heading = heading - (f32)(s32)(heading / TWO_PI) * TWO_PI;
        if (PI < heading) {
            heading -= TWO_PI;
        } else if (heading < -PI) {
            heading += TWO_PI;
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
    top = entity->model->position[1] - (entity->height + entity->params->attack_height_band) * 0.5f;
    bottom = entity->model->position[1] - (entity->height - entity->params->attack_height_band) * 0.5f;
    range = entity->params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        range *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        range *= 0.5f;
    }
    reach[0] = entity->params->ranged_attacks[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[0].reach_min : 0.0f;
    reach[1] = entity->params->ranged_attacks[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[1].reach_min : 0.0f;
    reach[2] = entity->params->ranged_attacks[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[2].reach_min : 0.0f;
    reach[3] = (range < entity->params->ranged_attacks[0].reach_max ? range : entity->params->ranged_attacks[0].reach_max) + entity->radius + 100.0f;
    reach[4] = (range < entity->params->ranged_attacks[1].reach_max ? range : entity->params->ranged_attacks[1].reach_max) + entity->radius + 100.0f;
    reach[5] = (range < entity->params->ranged_attacks[2].reach_max ? range : entity->params->ranged_attacks[2].reach_max) + entity->radius + 100.0f;
    if (top <= player_state.position.y && player_state.position.y - 230.0f <= bottom) {
        if (entity->params->ranged_attacks[0].record.ranged_weight != 0 && angle_off[0] < entity->params->ranged_attacks[0].widest_angle && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[3]) {
            return 1;
        }
        if (entity->params->ranged_attacks[1].record.ranged_weight != 0 && angle_off[1] < entity->params->ranged_attacks[1].widest_angle && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[4]) {
            return 1;
        }
        if (entity->params->ranged_attacks[2].record.ranged_weight != 0 && angle_off[2] < entity->params->ranged_attacks[2].widest_angle && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[5]) {
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

    angle = DEG_TO_RAD(ENTITY_PARAMS_RANGED(params)[index].heading_offset_deg);
    if (!(model->scale[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = wrap_angle(model->rotation[1] + angle);
    diff = wrap_angle(entity->heading_to_player - yaw);
    abs_diff = ABS(diff);

    height = entity->height;
    bottom = model->position[1] - (height + params->attack_height_band) * 0.5f;
    top = model->position[1] - (height - params->attack_height_band) * 0.5f;

    farthest = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        farthest *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        farthest *= 0.5f;
    }

    reach[0] = ENTITY_PARAMS_RANGED(params)[index].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[index].reach_min : 0.0f;
    reach[1] = (farthest < ENTITY_PARAMS_RANGED(params)[index].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[index].reach_max + entity->radius) + 100.0f;

    if (bottom <= player_state.position.y && player_state.position.y - 230.0f <= top && abs_diff < ENTITY_PARAMS_RANGED(params)[index].widest_angle &&
        reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[1]) {
        return TRUE;
    }
    return FALSE;
}

/* As EntityAi_CanUseRanged for the entity facing yaw. */
int EntityAi_CanUseRangedFacing(Entity *entity, float yaw, int index) {
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
    angle = DEG_TO_RAD(params->ranged_attacks[index].heading_offset_deg);
    yaw += angle;
    yaw -= (float)(int)(yaw / TWO_PI) * TWO_PI;
    if (PI < yaw) {
        yaw -= TWO_PI;
    } else if (yaw < -PI) {
        yaw += TWO_PI;
    }
    diff = entity->heading_to_player - yaw;
    diff -= (float)(int)(diff / TWO_PI) * TWO_PI;
    if (PI < diff) {
        diff -= TWO_PI;
    } else if (diff < -PI) {
        diff += TWO_PI;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    model = entity->model;
    high = model->position[1] - (entity->height + params->attack_height_band) * 0.5f;
    low = model->position[1] - (entity->height - params->attack_height_band) * 0.5f;
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    window[0] = params->ranged_attacks[index].reach_min != 0 ? entity->radius + 100.0f + params->ranged_attacks[index].reach_min : 0.0f;
    window[1] = (reach < params->ranged_attacks[index].reach_max ? reach : params->ranged_attacks[index].reach_max) + entity->radius + 100.0f;
    if (high <= player_state.position.y && player_state.position.y - 230.0f <= low && abs_diff < params->ranged_attacks[index].widest_angle) {
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
    reach = params->attack_range;
    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    // match: the window bounds re-read entity->params, which keeps a second copy of the pointer
    window[0] = entity->params->ranged_attacks[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[0].reach_min : 0.0f;
    window[1] = entity->params->ranged_attacks[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[1].reach_min : 0.0f;
    window[2] = entity->params->ranged_attacks[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->ranged_attacks[2].reach_min : 0.0f;
    window[3] = (reach < entity->params->ranged_attacks[0].reach_max ? reach : entity->params->ranged_attacks[0].reach_max) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->ranged_attacks[1].reach_max ? reach : entity->params->ranged_attacks[1].reach_max) + entity->radius + 100.0f;
    window[5] = (reach < params->ranged_attacks[2].reach_max ? reach : params->ranged_attacks[2].reach_max) + entity->radius + 100.0f;
    if ((params->ranged_attacks[0].record.ranged_weight != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->ranged_attacks[1].record.ranged_weight != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (params->ranged_attacks[2].record.ranged_weight != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
        return 1;
    }
    return 0;
}

/* Whether the player's distance lies in the window of ranged record index. */
static int EntityAi_InRangedDistanceOf(Entity *entity, int index) {
    EntityParams *params = entity->params;
    f32 reach = params->attack_range;
    f32 limits[2];
    f32 near_limit;

    if (entity->ailment_2_frames != 0 || (player_state.intimidate_timer > 0 && entity->distance_to_player_3d < player_state.intimidate_range)) {
        reach *= 0.5f;
    }
    if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        reach *= 0.5f;
    }
    if (params->ranged_attacks[index].reach_min != 0) {
        near_limit = entity->radius + 100.0f + params->ranged_attacks[index].reach_min;
    } else {
        near_limit = 0.0f;
    }
    limits[0] = near_limit;
    limits[1] = ((reach < params->ranged_attacks[index].reach_max) ? reach : params->ranged_attacks[index].reach_max) + entity->radius + 100.0f;
    if (limits[0] <= entity->distance_to_player && entity->distance_to_player < limits[1]) {
        return 1;
    }
    return 0;
}

/* Per-frame collision of an entity: clears its push and state bits, sets bits 22-27 from its params, then
   runs its entity, map object and map piece checks. */
void EntityAi_UpdateCollision(Entity *entity) {
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_LIFT_UP;
    entity->wall_push[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_LEDGE_STOP;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_WALL_PUSH;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_400000000000;
    entity->wall_push[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_PLAYER_PUSH;
    entity->wall_push[2] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_ENTITY_PUSH;
    entity->entity_push[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_OBJECT_PUSH;
    entity->entity_push[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~ENTITY_FLAG_ON_TOP;
    entity->entity_push[2] = 0;

    if (ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
        return;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_1)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_400000;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_WALLS)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_NO_WALL_COLLISION;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_SECOND_WALLS)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_NO_SECOND_WALL_COLLISION;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_MAP_OBJECTS)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_NO_MAP_OBJECT_COLLISION;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_ENTITIES)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_NO_ENTITY_COLLISION;
    }
    if (!(entity->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_PLAYER)) {
        ENTITY_FLAG_WORD(entity->flags) |= ENTITY_FLAG_NO_PLAYER_COLLISION;
    }

    EntityCollision_PushApart(entity);

    if (!((s32)(ENTITY_FLAG_WORD(entity->flags) >> 25) & ENTITY_FLAG_ACTIVE_LIST)) {
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

    if ((u32)(ENTITY_FLAG_WORD(entity->flags) >> 51) & ENTITY_FLAG_ACTIVE_LIST) {
        entity->floor_lift += 2.0f;
        if (40.0f < entity->floor_lift) {
            entity->floor_lift = 40.0f;
        }
    } else {
        entity->floor_lift = 10.0f;
    }
}

/* speed plus EntityParams.speed_step_per_frame, at most EntityParams.max_speed. */
float EntityAi_Accelerate(Entity *entity, float speed) {
    EntityParams *params = entity->params;
    float limit = params->max_speed;
    float result = speed + params->speed_step_per_frame;

    if (limit < result) {
        result = limit;
    }
    return result;
}

/* speed less EntityParams.speed_step_per_frame, at least 0. */
static float EntityAi_Decelerate(Entity *entity, float speed) {
    float result;

    result = speed - entity->params->speed_step_per_frame;
    if (result < 0.0f) {
        result = 0.0f;
    }
    return result;
}

/* Turns the entity's heading toward target at a rate that builds up to EntityParams.turn_limit; returns 1
   once it is there. */
s32 EntityAi_TurnToHeading(Entity *entity, f32 *speed, f32 target) {
    f32 diff;

    target -= (int)(target / TWO_PI) * TWO_PI;
    if (PI < target) {
        target -= TWO_PI;
    } else if (target < -PI) {
        target += TWO_PI;
    }
    if (entity->model->rotation[1] == target) {
        *speed = 0.0f;
        return 1;
    }
    diff = target - entity->model->rotation[1];
    diff -= (int)(diff / TWO_PI) * TWO_PI;
    if (PI < diff) {
        diff -= TWO_PI;
    } else if (diff < -PI) {
        diff += TWO_PI;
    }
    if (diff > 0.0f) {
        *speed += entity->params->turn_step_per_frame;
        if (*speed > entity->params->turn_limit) {
            *speed = entity->params->turn_limit;
        }
        if (diff <= *speed) {
            entity->turn_delta_yaw += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->turn_delta_yaw += *speed;
    } else if (diff < 0.0f) {
        *speed -= entity->params->turn_step_per_frame;
        if (*speed < -entity->params->turn_limit) {
            *speed = -entity->params->turn_limit;
        }
        if (*speed <= diff) {
            entity->turn_delta_yaw += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->turn_delta_yaw += *speed;
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
    angle += PI;
    angle -= (f32)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    angle -= (f32)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    if (entity->model->rotation[1] == angle) {
        *turn_rate = 0.0f;
        return 1;
    }
    diff = angle - entity->model->rotation[1];
    diff -= (f32)(int)(diff / TWO_PI) * TWO_PI;
    if (diff > PI) {
        diff -= TWO_PI;
    } else if (diff < -PI) {
        diff += TWO_PI;
    }
    if (diff > 0.0f) {
        *turn_rate += entity->params->turn_step_per_frame;
        if (*turn_rate > entity->params->turn_limit) {
            *turn_rate = entity->params->turn_limit;
        }
        if (diff <= *turn_rate) {
            entity->turn_delta_yaw += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->turn_delta_yaw += *turn_rate;
    } else if (diff < 0.0f) {
        *turn_rate -= entity->params->turn_step_per_frame;
        if (*turn_rate < -entity->params->turn_limit) {
            *turn_rate = -entity->params->turn_limit;
        }
        if (*turn_rate <= diff) {
            entity->turn_delta_yaw += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->turn_delta_yaw += *turn_rate;
    } else {
        *turn_rate = 0.0f;
        return 1;
    }
    return 0;
}

/* Sets the pause before the next attack (Entity.attack_delay_frames): base plus a share of the player's health in percent,
   more for each kind of healing item he carries. */
void EntityAi_SetAttackDelay(Entity *entity, s32 base) {
    s32 delay;

    delay = (player_state.hp_now * 100) / player_state.hp_max;
    delay = (item_counts.bytes[ITEM_HERB] == 0) ? delay : delay + 5;
    delay = (item_counts.bytes[ITEM_HERBAL_LIQUID] != 0) ? delay + 0xF : delay;
    delay = (item_counts.bytes[ITEM_FOREST_FRUIT] != 0) ? delay + 0x1E : delay;
    delay = (item_counts.bytes[ITEM_DRAGON_FLOWER] != 0) ? delay + 0x32 : delay;
    entity->attack_delay_frames = base + (delay * 0x1E) / 200;
}

/* Sets the entity's reaction, the handler that runs in place of its action until it clears itself (NULL
   clears it), and puts the reaction's step, Entity.reaction_step, back to 0; optionally runs it now. */
int EntityAi_SetReaction(Entity *entity, void (*reaction)(Entity *entity), int run_now) {
    entity->ai_state.reaction = reaction;
    entity->reaction_step = 0;
    if (run_now != 0) {
        if (reaction != NULL) {
            // match: retail calls it without passing the entity
            ((void (*)(void))reaction)();
        }
    }
    return 1;
}

/* Raises the weight factor (Entity.attack_weights) of the attack in flag bits 58-59 by 5, kept within 0x40..0xC0. */
void EntityAi_RaiseAttackWeight(Entity *entity) {
    u8 *level = entity->attack_weights;
    s32 value;
    s32 limit = 0xC0; // match: the 0xC0 cap stays in a register, as in EntityAi_LowerWeightsAfterHit

    level += ENTITY_FLAG_BITS(entity)->attack_index;
    value = *level + 5;
    if (value < 0x40) {
        value = 0x40;
    }
    if (value > limit) {
        value = limit;
    }
    *level = value;
}

/* Raises the weight factors of the three ranged attacks (Entity.attack_weights[3..5]) by 2, kept within 0x40..0xC0. */
void EntityAi_RaiseRangedWeights(Entity *entity) {
    adjust_level(&entity->attack_weights[3], 2, 0xC0);
    adjust_level(&entity->attack_weights[4], 2, 0xC0);
    adjust_level(&entity->attack_weights[5], 2, 0xC0);
}

/* The frame count of the motion animation index plays (Motion_FrameCount of EntityParams.motion_ids[index]). */
int EntityAi_AnimationFrameCount(Entity *entity, s32 index) {
    return Motion_FrameCount(ENTITY_MODEL(entity), entity->params->motion_ids[index]);
}

/* The frame count of motion index: MotionData.frame_count, the u16 in the four bytes in front of the part
   tables ModelInstance.level_tables[index] points at. */
u16 Motion_FrameCount(ModelInstance *model, int index) {
    ModelPartLevels *levels;

    /* match: indexing level_tables as an array swaps the addu operands */
    levels = *(ModelPartLevels **)((char *)model->level_tables + (index << 2));
    return *(u16 *)((char *)levels - 4);
}

/* Puts model part index in motion slot value (ModelPart.motion_slot), as Motion_SetPartSlots does for a run of
   parts. Nothing calls it. */
void Motion_SetPartSlot(ModelInstance *model, int index, u8 value) {
    model->parts[index].motion_slot = value;
}

/* Starts animation in a motion slot of the work area on channel, from frame, over frames (a quarter longer
   while Entity.ailment_3_frames runs and it is not busy); 1 without a change when the slot already plays it and force
   is 0. */
int EntityAi_StartSlotMotion(Entity *entity, EntityMotionSlotWord *slot, int channel, int animation, int frame, int frames, int force) {
    u8 *table = entity->params->motion_ids;
    int wanted = table[animation];
    int current = table[*(u8 *)slot & ENTITY_MOTION_SLOT_INDEX_MASK];

    if (force == 0 && current == wanted) {
        return 1;
    }
    slot->frame_fixed = frame << 8;
    slot->index = animation;
    slot->frame = frame;
    if (entity->ailment_3_frames != 0 && (entity->busy_flags & (ENTITY_BUSY_DEATH | ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY)) == 0) {
        float slowed = frames;

        slot->frames_left = slowed + slowed * 0.75f;
    } else {
        slot->frames_left = frames;
    }
    if (*(int *)slot & ENTITY_MOTION_SLOT_MOVES_ENTITY) {
        ModelPart *root = ENTITY_MODEL(entity)->root_part;

        root->position[0] = root->position[1] = root->position[2] = 0.0f;
    }
    return Motion_SeekSlotKeys(ENTITY_MODEL(entity), channel, wanted, frame);
}

/* Plays a motion slot a step on (Motion_BlendSlotToPose with weight) and returns its frames left, counted down by
   one. */
int EntityAi_StepSlotMotion(Entity *entity, EntityAiMotionSlotBits *slot, int channel, f32 weight) {
    u8 frames_left;

    Motion_BlendSlotToPose(ENTITY_MODEL(entity), channel, entity->params->motion_ids[slot->index], slot->frame, weight);
    slot->frames_left = frames_left = slot->frames_left - 1;
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

    GsDraw_SetScreenAlpha(0);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x1A, 0);
    Packet_AddGsRegister(0x1B, 0x248);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x55001);
    Packet_AddGsRegister(0x4B, 1);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        if (!ENTITY_FLAG_BITS(entity)->live || entity->map_record == NULL || (f32)entity->params->body_radius <= 0.0f) {
            continue;
        }
        if (!ENTITY_FLAG_BITS(entity)->draw_afterimage) {
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->map_record_inactive) {
            continue;
        }
        model = &entity_models[entity->pool_slot_index];
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
            ModelMatrixQwords *dst_matrix = (ModelMatrixQwords *)dst->matrix;
            ModelMatrixQwords *src_matrix = (ModelMatrixQwords *)src->matrix;

            dst_matrix->rows[0] = src_matrix->rows[0];
            dst_matrix->rows[1] = src_matrix->rows[1];
            dst_matrix->rows[2] = src_matrix->rows[2];
            dst_matrix->rows[3] = src_matrix->rows[3];
            dst++;
            src++;
        }
        Matrix_Multiply(model->model_matrix, ENTITY_MODEL(entity)->model_matrix, &fire_lord_model_matrix);
        model->light_block = ENTITY_MODEL(entity)->light_block;
        model->fog_block = ENTITY_MODEL(entity)->fog_block;
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_2, ((s64)(s32)(ENTITY_MODEL(entity)->alpha * 32.0f * (1.0f / 128.0f)) << 32) | 0x64);
        Packet_EndGsRegisters();
        model->draw_parts(model);
        GsDraw_QueueFlush();
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x1A, 1);
    Packet_AddGsRegister(0x4B, 0);
    Packet_EndGsRegisters();
}
