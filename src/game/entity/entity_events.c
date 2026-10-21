/* What an entity sets off: shots, effects and cues on the frames of its animations, its death and its return
   to its spawn point; and the turning of a model part (bone) towards a goal. */
#include "entity/entity_events.h"
#include "sound/sound_game.h"
#include "entity/entity_aim.h"
#include "effect/shot.h"
#include "effect/effect.h"
#include "player/player.h"
#include "entity/entity_world.h"
#include "item/gold.h"
#include "map/map.h"
#include "core/game.h"
#include "enemy/enemy.h"
#include "item/item.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include "script/script.h"

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

/* EntityMath_DistanceXZ inlined: the distance between two points, height left out */
static inline f32 flat_distance(f32 *from, f32 *target) {
    f32 distance_out[4];

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(from), "r"(target), "r"(distance_out) : "memory");
    return distance_out[0];
}

/* EntityMath_Approach inlined: `current` moved one step of `rate` towards `goal` */
static inline f32 approach_angle(f32 current, f32 goal, f32 rate) {
    f32 difference;
    f32 wrapped;
    s32 steps;

    if (current == goal) {
        return goal;
    }
    if (rate == 0.0f) {
        return current;
    }
    difference = goal - current;
    wrapped = difference - (f32)(s32)(difference / TWO_PI) * TWO_PI;
    if (wrapped > PI) {
        wrapped -= TWO_PI;
    } else if (wrapped < -PI) {
        wrapped += TWO_PI;
    }
    steps = (s32)(wrapped / rate);
    if (steps > 0) {
        current += rate;
    } else if (steps < 0) {
        current -= rate;
    } else {
        current = goal;
    }
    return wrap_angle(current);
}

/* EntityMath_EaseAngleToward, which the game inlines here: turns *angle towards `goal`, by the whole
   difference when `rate` is 1 or less, else by 1/rate of it */
static inline f32 turn_towards(f32 *angle, f32 goal, f32 rate) {
    f32 difference = goal - *angle;
    f32 turned;

    difference = difference - (f32)(s32)(difference / TWO_PI) * TWO_PI;
    if (difference > PI) {
        difference -= TWO_PI;
    } else if (difference < -PI) {
        difference += TWO_PI;
    }
    if (rate <= 1.0f) {
        *angle = goal;
        return difference;
    }
    turned = *angle + difference / rate;
    turned = turned - (f32)(s32)(turned / TWO_PI) * TWO_PI;
    if (turned > PI) {
        turned -= TWO_PI;
    } else if (turned < -PI) {
        turned += TWO_PI;
    }
    *angle = turned;
    return difference;
}

/* Launches the shots of a ranged record whose frame lies in (frame_from, frame_to], each from its bone along
   the bone, turned towards `target` by at most the two rates. */
int EntityShot_LaunchAimed(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;

    if (entity->ailment_4_frames != 0 && record->ignores_ailment_4 == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->launches[i].frame != 0 && frame_from < record->launches[i].frame && record->launches[i].frame <= frame_to) {
            shot = Shot_Alloc();
            shot->player_magic = shot->player_power = 0;
            shot->is_magic_shot = 1;
            shot->shot_params = entity->params->shot_params;
            from = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->launches[i].bone);
            tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->launches[i].bone + 1);
            if (from != NULL && tip != NULL) {
                if (record->launches[i].bone != 0xFF) {
                    delta_x = tip[0] - from[0];
                    delta_y = tip[1] - from[1];
                    delta_z = tip[2] - from[2];
                    angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + HALF_PI);
                    angles[2] = angles[3] = 0.0f;
                } else {
                    Vector_Copy(angles, entity->model->rotation);
                    angles[1] = wrap_angle(angles[1] + PI);
                }
                if (target != NULL) {
                    pitch = wrap_angle(-atan2(target[1] - from[1], flat_distance(from, target)));
                    yaw = atan2f(target[0] - from[0], target[2] - from[2]);
                    angles[0] = approach_angle(angles[0], pitch, pitch_rate);
                    angles[1] = approach_angle(angles[1], yaw, yaw_rate);
                }
                Attack_LaunchMagicEffect(record->launches[i].magic_effect, shot, from, angles, entity);
            }
        }
    }
    return 0;
}

/* Launches the shots of a ranged record whose frame lies in (frame_from, frame_to], from their bones along
   `direction`. */
int EntityShot_Launch(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *direction) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 *origin;
    s32 i;

    if (entity->ailment_4_frames != 0 && record->ignores_ailment_4 == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->launches[i].frame != 0 && frame_from < record->launches[i].frame && record->launches[i].frame <= frame_to) {
            shot = Shot_Alloc();
            shot->is_magic_shot = 1;
            shot->player_power = 0;
            shot->player_magic = 0;
            shot->shot_params = entity->params->shot_params;
            origin = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->launches[i].bone);
            if (origin != NULL) {
                Attack_LaunchMagicEffect(record->launches[i].magic_effect, shot, origin, direction, entity);
            }
        }
    }
    return 0;
}

/* Starts effect `effect_id` at point `point_id` of the entity's model, turned by `rotation`. */
void EntityEvent_StartEffectAtPoint(Entity *entity, int effect_id, u32 point_id, f32 *rotation) {
    f32 *point;

    point = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), point_id);
    if (point != 0) {
        Effect_StartSpawner(effect_id, point, rotation, entity);
    }
}

/* Starts the effects animation `animation` lists for the frames from `frame_from` up to `frame_to` (both in
   1/256 frames), each at a bone of the model, pointing along it; only while the entity stands on a map record
   with flag 0x4000000. */
void EntityEvent_StartFrameEffects(Entity *entity, s32 animation, s32 frame_from, s32 frame_to) {
    EntityParamsUnk130 *record;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 frame;
    s32 i;

    record = (EntityParamsUnk130 *)(entity_params + entity->params->anim_effect_offsets[animation]);
    count = entity->params->anim_effect_counts[animation];
    if (count <= 0) {
        return;
    }
    if (record == NULL) {
        return;
    }
    if (entity->map_record == NULL) {
        return;
    }
    if (!(((MapRecord *)entity->map_record)->record_flags & MAP_RECORD_FRAME_EFFECTS)) {
        return;
    }
    for (i = 0; i < count; i++, record++) {
        from = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->bone);
        tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->bone + 1);
        if (from != NULL && tip != NULL) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + HALF_PI);
            angles[2] = angles[3] = 0.0f;
            frame = record->frame << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (record->use_variant != 0) {
                Effect_StartSpawner(entity->params->variant_effect_ids[ENTITY_FLAG_BITS(entity)->floor_material], from, angles, entity);
            } else {
                Effect_StartSpawner(record->effect_id, from, angles, entity);
            }
        }
    }
}

/* Passes to Sound_PlayFalloff what animation `animation` lists for the frames from `frame_from` up to `frame_to`
   (both in 1/256 frames), with where the entity is as seen from the player. */
void EntityEvent_StartFrameCues(Entity *entity, s32 animation, s32 frame_from, s32 frame_to) {
    EntityParamsUnk1B0 *record;
    s32 count;
    s32 i;
    s32 frame;
    f32 angle;

    record = (EntityParamsUnk1B0 *)(entity_params + entity->params->anim_sound_offsets[animation]);
    count = entity->params->anim_sound_counts[animation];
    if (count > 0 && record != NULL) {
        angle = atan2f(entity->model->position[0] - player_state.position.x, entity->model->position[2] - player_state.position.z) - player_state.rotation.y;
        angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
        if (angle > PI) {
            angle -= TWO_PI;
        } else if (angle < -PI) {
            angle += TWO_PI;
        }
        for (i = 0; i < count; i++, record++) {
            frame = record->frame << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (entity->ailment_4_frames != 0 && record->skip_while_ailing != 0) {
                continue;
            }
            Sound_PlayFalloff(record->sound, record->kind, entity->sound_bank, entity->params->sound_id, entity->distance_to_player_3d, angle);
        }
    }
}

/* Passes a cue to Sound_PlayFalloff with where the entity is as seen from the player; not while the ailment_4_frames
   timer runs if `skip_while_ailing` is set. */
void EntityEvent_StartCue(Entity *entity, s32 sound_index, s32 falloff_kind, s32 skip_while_ailing) {
    f32 angle;

    angle = atan2f(entity->model->position[0] - player_state.position.x, entity->model->position[2] - player_state.position.z) - player_state.rotation.y;
    angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    if (entity->ailment_4_frames != 0 && skip_while_ailing != 0) {
        return;
    }
    Sound_PlayFalloff((u8)sound_index, (u8)falloff_kind, entity->sound_bank, entity->params->sound_id, entity->distance_to_player_3d, angle);
}

/* As EntityEvent_StartCue, without the ailment_4_frames test. */
void EntityEvent_StartCueAlways(Entity *entity, s32 sound_index, s32 falloff_kind) {
    f32 angle;
    f32 wrapped;

    angle = atan2f(entity->model->position[0] - player_state.position.x, entity->model->position[2] - player_state.position.z);
    angle -= player_state.rotation.y;
    wrapped = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI;
    if (wrapped > PI) {
        wrapped -= TWO_PI;
    } else if (wrapped < -PI) {
        wrapped += TWO_PI;
    }
    Sound_PlayFalloff((u8)sound_index, (u8)falloff_kind, entity->sound_bank, entity->params->sound_id, entity->distance_to_player_3d, wrapped);
}

/* Puts the entity back at its spawn point (Entity_ResetToSpawn), sets flag bits 1 and 8 and clears bits 2-3,
   frees its effects and starts the effect EntityParams.respawn_effect_id names at bone unkC5. */
void EntityEvent_Respawn(Entity *entity) {
    f32 *bone;

    Entity_ResetToSpawn(entity);
    entity->flags = ((entity->flags | ENTITY_FLAG_LIVE) & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_RESPAWN_ROLL_DONE;
    Effect_FreeForOwner(entity);
    if (entity->params->respawn_effect_id != 0) {
        if (entity->params->respawn_effect_id != 0xFFFF) {
            bone = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), entity->params->respawn_effect_bone);
            Effect_StartSpawner(entity->params->respawn_effect_id, bone, (char *)entity->model + 0xD0, entity);
        }
    }
}

/* What every type does when its entity dies: drops its item and gold, gives the player its experience when
   the player killed it (flag bits 38-39), passes the map tag and spawn index to Game_CountSpawnDeath, frees its
   effects and starts the spawn's script. */
void EntityEvent_OnDeath(Entity *entity) {
    MapItem_DropFromEnemy(entity);
    Gold_DropFromEntity(entity);
    if (ENTITY_FLAG_HIT_BITS(entity)->hit_source) {
        Player_AddExperience(entity->params->experience_reward);
    }
    Game_CountSpawnDeath(map_slots[entity->group_id].tag, entity->spawn_info - map_spawn_infos[entity->group_id]);
    Effect_FreeForOwner(entity);
    if (entity->spawn_info->death_script_id != 0 && entity->group_id == current_map_slot) {
        Script_Start(entity->spawn_info->death_script_id, 0);
    }
}

/* Turns the three angles of a model part. mode 1: `angles` towards `goal` (kept within pitch
   limits[0]..limits[1] and yaw limits[2]..limits[3] if given) and into the part; mode 0: `angles` towards the
   part's own and into the part; else `angles` are read from the part. FALSE: no part. */
s32 Bone_TurnPart(ModelPart *part, f32 *angles, s32 mode, f32 *goal, f32 *limits, f32 rate) {
    if (part == NULL) {
        return FALSE;
    }
    if (mode == 1) {
        if (limits != NULL) {
            if (goal[0] < limits[0]) {
                goal[0] = limits[0];
            }
            if (limits[1] < goal[0]) {
                goal[0] = limits[1];
            }
            if (goal[1] < limits[2]) {
                goal[1] = limits[2];
            }
            if (limits[3] < goal[1]) {
                goal[1] = limits[3];
            }
        }
        turn_towards(&angles[0], goal[0], rate);
        turn_towards(&angles[1], goal[1], rate);
        turn_towards(&angles[2], goal[2], rate);
        Vector_Copy(&part->rotation_x, angles);
    } else if (mode == 0) {
        turn_towards(&angles[0], part->rotation_x, rate);
        turn_towards(&angles[1], part->rotation_y, rate);
        turn_towards(&angles[2], part->rotation_z, rate);
        Vector_Copy(&part->rotation_x, angles);
    } else {
        Vector_Copy(angles, &part->rotation_x);
    }
    return TRUE;
}

/* Bone_TurnPart for a part that is to look at a point: in mode 1 `angles` turn towards what Bone_LookAngles
   gives for `target` (FALSE if it gives none). FALSE: no model or no part. */
int Bone_TurnPartToLook(void *model, int part_arg, int angles_arg, int mode, void *target, int limits_arg, float rate) {
    ModelPart *part = (ModelPart *)part_arg;
    f32 *angles = (f32 *)angles_arg;
    f32 goal[4];

    if (model == NULL || part == NULL) {
        return FALSE;
    }
    if (mode == 1) {
        if (Bone_LookAngles(model, part, target, (f32 *)limits_arg, goal) == 0) {
            return FALSE;
        }
        turn_towards(&angles[0], goal[0], rate);
        turn_towards(&angles[1], goal[1], rate);
        turn_towards(&angles[2], goal[2], rate);
        Vector_Copy(&part->rotation_x, angles);
    } else if (mode == 0) {
        turn_towards(&angles[0], part->rotation_x, rate);
        turn_towards(&angles[1], part->rotation_y, rate);
        turn_towards(&angles[2], part->rotation_z, rate);
        Vector_Copy(&part->rotation_x, angles);
    } else {
        Vector_Copy(angles, &part->rotation_x);
    }
    return TRUE;
}
