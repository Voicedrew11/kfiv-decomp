/* What an entity sets off: shots, effects and cues on animation frames, its death, its return to its spawn
   point; and the turning of a model part towards a goal (kf_entity_events.c). */
#ifndef KF_ENTITY_EVENTS_H
#define KF_ENTITY_EVENTS_H

#include "common.h"
#include "kf_entity.h"

/* Launches the shots of a ranged record due in (frame_from, frame_to], turned towards `target`. */
int EntityShot_LaunchAimed(Entity *entity, EntityParamsUnk3D8 *record, s32 frame_from, s32 frame_to, f32 *target,
                           f32 pitch_rate, f32 yaw_rate);

/* Launches the shots of a ranged record due in (frame_from, frame_to] along `direction`. */
int EntityShot_Launch(Entity *entity, EntityParamsUnk3D8 *record, s32 frame_from, s32 frame_to, f32 *direction);

/* Starts effect `effect_id` at point `point_id` of the entity's model, turned by `rotation`. */
void EntityEvent_StartEffectAtPoint(char **entity, int effect_id, u32 point_id, int rotation);

/* Starts the effects animation `animation` lists for the frames in [frame_from, frame_to). */
void EntityEvent_StartFrameEffects(Entity *entity, s32 animation, s32 frame_from, s32 frame_to);

/* Passes to Sound_PlayFalloff what animation `animation` lists for the frames in [frame_from, frame_to). */
void EntityEvent_StartFrameCues(Entity *entity, s32 animation, s32 frame_from, s32 frame_to);

/* Passes a cue to Sound_PlayFalloff with where the entity is as seen from the player. */
void EntityEvent_StartCue(Entity *entity, s32 unknown1, s32 unknown2, s32 skip_if_unk2A);

/* EntityEvent_StartCue without the unk2A test. */
void EntityEvent_StartCueAlways(Entity *entity, s32 unknown1, s32 unknown2);

/* Puts the entity back at its spawn point. */
void EntityEvent_Respawn(Entity *entity);

/* What every type does when its entity dies. */
void EntityEvent_OnDeath(Entity *entity);

/* Turns the three angles of a model part (mode 1: towards `goal`; mode 0: towards the part's own). */
s32 Bone_TurnPart(ModelPart *part, f32 *angles, s32 mode, f32 *goal, f32 *limits, f32 rate);

/* Bone_TurnPart for a part that is to look at `target`. */
int Bone_TurnPartToLook(void *model, int part, int angles, int mode, void *target, int limits, float rate);

#endif
