/* The shared entity AI helpers (kf_entity_ai.c): handler switching, attack reach tests,
   turning, the attack delay, and the motion slots of the work area. */
#ifndef KF_ENTITY_AI_H
#define KF_ENTITY_AI_H

#include "common.h"
#include "kf_entity.h"
#include "kf_model.h"

/* EntityMotionSlot (kf_entity.h) as EntityAi_StartSlotMotion fills it: the bit-fields in one word */
typedef struct {
    u32 index : 5;                  /* 0x00 which EntityParams.unk60 entry (motion) it plays */
    u32 moves_entity : 1;           /* the root part's movement in the motion is handed to the entity */
    u32 bit6 : 1;
    u32 backwards : 1;
    u32 unk01 : 8;                  /* 0x01 frames left, counted down by EntityAi_StepSlotMotion */
    u32 frame : 16;                 /* 0x02 */
    s32 frame_fixed;                /* 0x04 the frame times 256 */
} EntityMotionSlotWord;

/* A random whole number of degrees as an angle in radians, wrapped to -pi..pi */
float EntityAi_RandomAngle(void);
/* Sets D_004325A0[30] to float_value and D_004325A0[31] to int_value */
void func_00195878(int float_value, int int_value);
/* Sets Entity.state and keeps the old value in prev_state */
void EntityAi_SetState(Entity *entity, u8 state);
/* Installs the entity's per-frame update (Entity.unk50; the type table D_003E6E40 gives it) and puts
   action_step back to 0 */
void EntityAi_SetUpdate(Entity *entity, s32 update);
/* Switches the entity's handler, keeping the old one as prev_handler; optionally runs the new one now */
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
/* Switches the entity to handler (keeping the old one), sets flag bit 57 and bits 58-59, optionally runs it */
int EntityAi_StartAttack(Entity *entity, void (*handler)(Entity *entity), int run, int bit57, int bits58);
/* Hit check of one part of a melee record: turns the model's heading by the record's angle (mirrored when
   the model is) and hands it to EntityAi_CheckMeleeHitAt */
int EntityAi_CheckMeleeHit(Entity *entity, EntityParamsUnk240 *record, int part, int frame_from, int frame_to);
/* Runs EntityAi_SendMeleeHit for one part of a melee record facing angle, unless the animation frames
   [frame_from, frame_to] lie outside the frames the part is active on (0 then) */
s32 EntityAi_CheckMeleeHitAt(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle);
/* Builds the attack request of one part of a melee record (reach, height, angle, power) and runs it with
   func_001B1470; returns its result */
s32 EntityAi_SendMeleeHit(Entity *entity, EntityParamsUnk240 *record, s32 index, f32 angle);
/* Allocates a shot carrying EntityParams.unk238 and launches effect effect_id from origin along angles
   (func_001B4F30) for the entity */
int EntityAi_LaunchShot(Entity *entity, int effect_id, void *origin, void *angles);
/* EntityShot_LaunchAimed with no target: launches the shots of a ranged record whose frame lies in (frame_from,
   frame_to] */
int EntityAi_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 elapsed, s32 unk82);
/* Spawns an effect at a model point, facing along the line to the next point */
void EntityAi_SpawnBoneEffect(char **model, int effect_id, int point);
/* Turns the model part at EntityModel.unk138 to look at target (Bone_TurnPartToLook) while params flag 0x2000 is
   set; FALSE without it */
int EntityAi_LookAtPoint(Entity *entity, int angles, int mode, int target, int limits, float rate);
/* As EntityAi_LookAtPoint, at the player's position 220 above his feet */
int EntityAi_LookAtPlayer(Entity *entity, void *target, s32 mode, void *out, f32 range);
/* Once the model is below y 10000 (fallen out of the map): frees its effects, clears flag bit 1 and returns
   1; else 0 */
s32 EntityAi_RemoveIfFallen(Entity *entity);
/* Decides whether a dead entity comes back: it must be allowed to, have deaths left, and the player must
   have come within its respawn distance, at which point one roll against its respawn chance is made */
s32 EntityAi_ShouldRespawn(Entity *entity);
/* Sets flag 32 while the player is within reach, arc and height, and flag 33 (noticed) by a chance that
   grows as he comes nearer; flag 63 when something stands between them, which clears flag 32 (mode below 2)
   and flag 33 (mode 0) */
void EntityAi_SensePlayer(Entity *actor, s32 mode);
/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.unk4C (in x, y and z),
   clears it otherwise */
void EntityAi_CheckFarFromSpawn3D(Entity *entity);
/* Sets flag 53 while the model is farther from its spawn point than EntitySpawnInfo.unk4C (in x and z),
   clears it otherwise */
void EntityAi_CheckFarFromSpawn(Entity *entity);
/* Clears flag bit 31 */
void func_00196D48(Entity *entity);
/* Tests whether the target is within a distance and inside the horizontal and vertical view cones (each
   given as a full angle); a limit of 0 skips that test */
s32 EntityAi_IsPointInView(Entity *entity, f32 *target, f32 yaw_range, f32 pitch_range, f32 max_distance);
/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three melee or
   three ranged attacks */
s32 EntityAi_CanAttack(Entity *entity);
/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three melee
   attacks */
s32 EntityAi_CanMelee(Entity *entity);
/* Whether the player is inside the reach (distance, angle and height) of melee record index */
int EntityAi_CanUseMelee(Entity *entity, int index);
/* As EntityAi_CanUseMelee for the entity facing yaw */
int EntityAi_CanUseMeleeFacing(Entity *entity, int index, float yaw);
/* Whether the player is inside the reach (distance, angle and height) of any of the entity's three ranged
   attacks */
s32 EntityAi_CanShoot(Entity *entity);
/* Whether the player is inside the reach (distance, angle and height) of ranged record index */
s32 EntityAi_CanUseRanged(Entity *entity, s32 index);
/* As EntityAi_CanUseRanged for the entity facing yaw */
int EntityAi_CanUseRangedFacing(Entity *entity, int index, float yaw);
/* Per-frame collision of an entity: clears its push and state bits, sets bits 22-27 from its params, then
   runs its entity, map object and map piece checks */
void EntityAi_UpdateCollision(Entity *entity);
/* speed plus EntityParams.unk50, at most EntityParams.unk54 */
float EntityAi_Accelerate(Entity *entity, float speed);
/* Turns the entity's heading toward target at a rate that builds up to EntityParams.turn_limit; returns 1
   once it is there */
s32 EntityAi_TurnToHeading(Entity *entity, f32 *speed, f32 target);
/* As EntityAi_TurnToHeading, toward a point */
int EntityAi_TurnToPoint(Entity *entity, f32 *turn_rate, f32 *target);
/* Sets the pause before the next attack (Entity.unkA2): base plus a share of the player's health in percent,
   more for each kind of healing item he carries */
void EntityAi_SetAttackDelay(Entity *entity, s32 base);
/* Sets the entity's reaction, the handler that runs in place of its action until it clears itself (NULL
   clears it), and puts the reaction's step, Entity.unkCB, back to 0; optionally runs it now */
int EntityAi_SetReaction(Entity *entity, void (*reaction)(void), int run_now);
/* Raises the weight factor (Entity.unkC0) of the attack in flag bits 58-59 by 5, kept within 0x40..0xC0 */
void EntityAi_RaiseAttackWeight(Entity *entity);
/* Raises the weight factors of the three ranged attacks (Entity.unkC0[3..5]) by 2, kept within 0x40..0xC0 */
void EntityAi_RaiseRangedWeights(Entity *entity);
/* func_0019A418 for the motion of animation index (EntityParams.unk60) */
int func_0019A3F0(Entity *entity, s32 index);
/* The u16 in the four bytes in front of the block ModelInstance.level_tables[index] points at */
u16 func_0019A418(ModelInstance *model, int index);
/* Sets byte unk6C of model part index */
void func_0019A430(ModelInstance *model, int index, u8 value);
/* Starts animation in a motion slot of the work area on channel, from frame, over frames (a quarter longer
   while Entity.unk28 runs and it is not busy); 1 without a change when the slot already plays it and force
   is 0 */
int EntityAi_StartSlotMotion(Entity *entity, EntityMotionSlotWord *slot, int channel, int animation, int frame, int frames, int force);
/* Plays a motion slot a step on (Motion_BlendSlotToPose with weight) and returns its frames left, counted down by one */
int EntityAi_StepSlotMotion(Entity *entity, EntityMotionSlot *slot, int channel, f32 weight);
/* Cubic Hermite interpolation: weights the two points and two tangents by the Hermite basis at ratio and
   hands them to Vector_WeightedSum4 */
void Vector_Hermite(void *result, void *point0, void *point1, void *tangent0, void *tangent1, f32 ratio);

#endif
