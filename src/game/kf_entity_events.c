/* What an entity sets off: shots, effects and cues on the frames of its animations, its death and its return
   to its spawn point; and the turning of a model part (bone) towards a goal. */
#include "kf_entity_events.h"
#include "kf_entity_aim.h"
#include "kf_shot.h"
#include "kf_effect.h"
#include "kf_player.h"
#include "kf_entity_world.h"
#include "kf_gold.h"
#include "kf_map.h"

float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
void Vector_Copy(void *, void *);
int func_001B4F30(u16, void *, void *, void *, void *);
extern char *D_004467C0;
extern PlayerState D_00413F80;
// match: Sound_PlayFalloff.c has not been given its real arguments yet
void Sound_PlayFalloff(float, int, int, char, float, float);
extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
extern int D_00446770;
s32 func_001C8CB0(Entity *);
void Player_AddExperience(s32);
void func_001035A8(u32, int);
int Script_Start(int, int);

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
    wrapped = difference - (f32)(s32)(difference / 6.2831855f) * 6.2831855f;
    if (wrapped > 3.1415927f) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
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

    difference = difference - (f32)(s32)(difference / 6.2831855f) * 6.2831855f;
    if (difference > 3.1415927f) {
        difference -= 6.2831855f;
    } else if (difference < -3.1415927f) {
        difference += 6.2831855f;
    }
    if (rate <= 1.0f) {
        *angle = goal;
        return difference;
    }
    turned = *angle + difference / rate;
    turned = turned - (f32)(s32)(turned / 6.2831855f) * 6.2831855f;
    if (turned > 3.1415927f) {
        turned -= 6.2831855f;
    } else if (turned < -3.1415927f) {
        turned += 6.2831855f;
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

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->unk10[i].unk02 != 0 && frame_from < record->unk10[i].unk02 && record->unk10[i].unk02 <= frame_to) {
            shot = Shot_Alloc();
            shot->unk22 = shot->unk20 = 0;
            shot->unk00 = 1;
            shot->unk28 = entity->params->unk238;
            from = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
            tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01 + 1);
            if (from != NULL && tip != NULL) {
                if (record->unk10[i].unk01 != 0xFF) {
                    delta_x = tip[0] - from[0];
                    delta_y = tip[1] - from[1];
                    delta_z = tip[2] - from[2];
                    angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles[2] = angles[3] = 0.0f;
                } else {
                    Vector_Copy(angles, entity->model->unkD0);
                    angles[1] = wrap_angle(angles[1] + 3.1415927f);
                }
                if (target != NULL) {
                    pitch = wrap_angle(-atan2(target[1] - from[1], flat_distance(from, target)));
                    yaw = atan2f(target[0] - from[0], target[2] - from[2]);
                    angles[0] = approach_angle(angles[0], pitch, pitch_rate);
                    angles[1] = approach_angle(angles[1], yaw, yaw_rate);
                }
                func_001B4F30(record->unk10[i].unk00, shot, from, angles, entity);
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
    char *origin;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->unk10[i].unk02 != 0 && frame_from < record->unk10[i].unk02 && record->unk10[i].unk02 <= frame_to) {
            shot = Shot_Alloc();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            shot->unk28 = entity->params->unk238;
            origin = func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
            if (origin != NULL) {
                func_001B4F30(record->unk10[i].unk00, shot, origin, direction, entity);
            }
        }
    }
    return 0;
}

/* Starts effect `effect_id` at point `point_id` of the entity's model, turned by `rotation`. */
void EntityEvent_StartEffectAtPoint(char **entity, int effect_id, u32 point_id, int rotation) {
    char *point;

    point = func_0010CAB8(*entity, point_id);
    if (point != 0) {
        Effect_StartSpawner(effect_id, point, (void *)rotation, entity);
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

    record = (EntityParamsUnk130 *)(D_004467C0 + entity->params->unk130[animation]);
    count = entity->params->unkF0[animation];
    if (count <= 0) {
        return;
    }
    if (record == NULL) {
        return;
    }
    if (entity->unk14 == NULL) {
        return;
    }
    if (!(((MapRecord *)entity->unk14)->unk38 & 0x4000000)) {
        return;
    }
    for (i = 0; i < count; i++, record++) {
        from = (f32 *)func_0010CAB8((char *)entity->model, record->unk04);
        tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk04 + 1);
        if (from != NULL && tip != NULL) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + 1.5707964f);
            angles[2] = angles[3] = 0.0f;
            frame = record->unk02 << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (record->unk05 != 0) {
                Effect_StartSpawner(entity->params->unk440[(s32)(entity->flags >> 60) & 7], from, angles, entity);
            } else {
                Effect_StartSpawner(record->unk00, from, angles, entity);
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

    record = (EntityParamsUnk1B0 *)(D_004467C0 + entity->params->unk1B0[animation]);
    count = entity->params->unk110[animation];
    if (count > 0 && record != NULL) {
        angle = atan2f(entity->model->position[0] - D_00413F80.position.x, entity->model->position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (angle > 3.1415927f) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        for (i = 0; i < count; i++, record++) {
            frame = record->unk02 << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (entity->unk2A != 0 && record->unk04 != 0) {
                continue;
            }
            Sound_PlayFalloff(entity->params->unk42E, record->unk00, record->unk01, entity->unkD2, entity->unk60, angle);
        }
    }
}

/* Passes a cue to Sound_PlayFalloff with where the entity is as seen from the player; not while the unk2A timer
   runs if `skip_if_unk2A` is set. */
void EntityEvent_StartCue(Entity *entity, s32 unknown1, s32 unknown2, s32 skip_if_unk2A) {
    f32 angle;

    angle = atan2f(entity->model->position[0] - D_00413F80.position.x, entity->model->position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->unk2A != 0 && skip_if_unk2A != 0) {
        return;
    }
    Sound_PlayFalloff(entity->params->unk42E, (u8)unknown1, (u8)unknown2, entity->unkD2, entity->unk60, angle);
}

/* As EntityEvent_StartCue, without the unk2A test. */
void EntityEvent_StartCueAlways(Entity *entity, s32 unknown1, s32 unknown2) {
    f32 angle;
    f32 wrapped;

    angle = atan2f(entity->model->position[0] - D_00413F80.position.x, entity->model->position[2] - D_00413F80.position.z);
    angle -= D_00413F80.rotation.y;
    wrapped = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (wrapped > 3.1415927f) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    Sound_PlayFalloff(entity->params->unk42E, (u8)unknown1, (u8)unknown2, entity->unkD2, entity->unk60, wrapped);
}

/* Puts the entity back at its spawn point (Entity_ResetToSpawn), sets flag bits 1 and 8 and clears bits 2-3,
   frees its effects and starts the effect EntityParams.unkC6 names at bone unkC5. */
void EntityEvent_Respawn(Entity *entity) {
    char *bone;

    Entity_ResetToSpawn(entity);
    entity->flags = ((entity->flags | 2) & ~0xC) | 0x100;
    Effect_FreeForOwner(entity);
    if (entity->params->unkC6 != 0) {
        if (entity->params->unkC6 != 0xFFFF) {
            bone = func_0010CAB8((char *)entity->model, entity->params->unkC5);
            Effect_StartSpawner(entity->params->unkC6, bone, (char *)entity->model + 0xD0, entity);
        }
    }
}

/* What every type does when its entity dies: drops its item and gold, gives the player its experience when
   the player killed it (flag bits 38-39), passes the map tag and spawn index to func_001035A8, frees its
   effects and starts the spawn's script. */
void EntityEvent_OnDeath(Entity *entity) {
    func_001C8CB0(entity);
    Gold_DropFromEntity(entity);
    if ((s32)(entity->flags >> 38) & 3) {
        Player_AddExperience(entity->params->unk0E);
    }
    func_001035A8(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
    Effect_FreeForOwner(entity);
    if (entity->spawn_info->unk36 != 0 && entity->group_id == D_00446770) {
        Script_Start(entity->spawn_info->unk36, 0);
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
        Vector_Copy(&part->unk50, angles);
    } else if (mode == 0) {
        turn_towards(&angles[0], part->unk50, rate);
        turn_towards(&angles[1], part->unk54, rate);
        turn_towards(&angles[2], part->unk58, rate);
        Vector_Copy(&part->unk50, angles);
    } else {
        Vector_Copy(angles, &part->unk50);
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
        Vector_Copy(&part->unk50, angles);
    } else if (mode == 0) {
        turn_towards(&angles[0], part->unk50, rate);
        turn_towards(&angles[1], part->unk54, rate);
        turn_towards(&angles[2], part->unk58, rate);
        Vector_Copy(&part->unk50, angles);
    } else {
        Vector_Copy(angles, &part->unk50);
    }
    return TRUE;
}
