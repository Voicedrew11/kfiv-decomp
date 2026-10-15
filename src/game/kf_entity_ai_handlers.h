/* The shared entity AI: the per-frame update most entity types run (EntityAi_Update), the set-up of type 0,
   and the default handlers a type's set-up function installs in the work area (think, animate, on_hit,
   dormant, death, move), with the actions and reactions they start. See EntityUnkE0 in kf_entity.h. */
#ifndef KF_ENTITY_AI_HANDLERS_H
#define KF_ENTITY_AI_HANDLERS_H

#include "common.h"
#include "kf_entity.h"

/* An action that lets the current motion play to its end, then thinks. */
void EntityAi_FinishMotion(Entity *entity, s32 unused);

/* An action (busy bit 0x200): plays animation 0 (standing) once to its end, then thinks. */
void EntityAi_StandOnce(Entity *entity);

/* The default dormant action (busy bit 0x20): waits for the player, gets up, then state 2. */
void EntityAi_Dormant(Entity *entity);

/* The reaction to a light hit during flag bit 19: the current animation three frames back. */
void EntityAi_ReactionRewind(Entity *entity);

/* The reaction to a light hit (busy bit 2): animation 14. */
void EntityAi_ReactionLight(Entity *entity);

/* The reaction to a heavy hit (busy bit 4): animation 15. */
void EntityAi_ReactionHeavy(Entity *entity);

/* The default death action (busy bit 0x40): animation 16, then fades the model out. */
void EntityAi_Death(Entity *entity);

/* The default dodge action (busy bit 8): animation 10, its root motion moving the entity. */
void EntityAi_Dodge(Entity *entity);

/* The default guard action (busy bit 0x10): animation 13, held with flag bit 35 set. */
void EntityAi_Guard(Entity *entity);

/* The default melee attack (busy bit 1): animation 20 + n for attack record n. */
void EntityAi_MeleeAttack(Entity *entity);

/* The default ranged attack (busy bit 1): animation 23 + n for ranged record n. */
void EntityAi_RangedAttack(Entity *entity);

/* The per-frame update of 82 entity types: handlers, timers, animation and this frame's move. */
void EntityAi_Update(Entity *entity);

/* The set-up function of entity type 0: the work area with the default handlers. */
void EntityAi_InitDefaults(Entity *entity);

/* The default animate handler: plays the motion a step on and turns the head. */
void EntityAi_Animate(Entity *entity);

/* The default on_hit handler: forgets the target and starts the reaction the hit calls for. */
void EntityAi_OnHit(Entity *entity);

/* The default think handler: every tenth frame updates what the entity knows of the player (EntityAi_SensePlayer). */
void EntityAi_Think(Entity *entity, s32 unknown);

/* The think function while flag bit 53 is set: walks back to the spawn point. */
int EntityAi_ThinkReturn(void *entity_ptr, int unknown);

/* The think function of state 0: wanders near the spawn point. */
int EntityAi_ThinkIdle(void *entity_ptr, int unknown);

/* The think function of state 1 (the player was noticed and is no longer): keeps walking to the player. */
void EntityAi_ThinkPursue(Entity *entity, int extra);

/* The think function of state 2: guards, dodges, attacks, closes in or waits. */
void EntityAi_ThinkCombat(Entity *entity, s32 unknown);

/* The default move action: carries out the command in Entity.unkC8 and picks the animation. */
void EntityAi_Move(Entity *entity);

#endif
