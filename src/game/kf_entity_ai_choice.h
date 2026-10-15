/* The choices of the shared entity AI: what the entity knows of the player (Entity.state), guarding and
   dodging, which attack and which combat choice to take and how their weights change, and the movement
   commands (Entity.unkC8) that take it towards its target or aside from a push. */
#ifndef KF_ENTITY_AI_CHOICE_H
#define KF_ENTITY_AI_CHOICE_H

#include "common.h"
#include "kf_entity.h"

/* Steps Entity.state from flag bit 33: 0 not noticed, 2 noticed, 1 noticed a while ago. */
void EntityAi_StepState(Entity *entity);

/* The heading past side on to a push from the front; returns the side (1, -1) or 0. */
int EntityAi_TurnFromPush(void *entity_arg, void *heading_out);

/* Whether the push the entity is under (flags 45, 46, 48) comes against `heading` (from the front). */
s32 EntityAi_IsPushedFromFront(Entity *entity, f32 heading);

/* Whether to guard against the player's swing (chance EntityParams.unkCB / 5 percent). */
s32 EntityAi_ShouldGuard(Entity *entity, f32 max_distance, f32 max_angle);

/* As EntityAi_ShouldGuard, by a chance of EntityParams.unkCA / 5 percent. */
s32 EntityAi_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle);

/* Picks one of the six attacks the player is in reach of; returns flag bit 57 (ranged). */
s32 EntityAi_PickAttack(Entity *entity);

/* Picks one of the three melee attacks the player is in reach of; returns it. */
s32 EntityAi_PickMeleeAttack(Entity *entity);

/* Picks one of the three ranged attacks the player is in reach of; returns it. */
s32 EntityAi_PickRangedAttack(Entity *entity);

/* Picks the next of the four combat choices (Entity.unkCD. */
s32 EntityAi_PickCombatChoice(Entity *entity, s32 mode);

/* After a hit in state 2, makes the choice (and attack) the entity was on less likely. */
void EntityAi_LowerWeightsAfterHit(Entity *entity);

/* Lowers the weight (unkC0) of the attack flag bits 57-59 name by 10, kept between 0x40 and 0xC0. */
void EntityAi_LowerAttackWeight(Entity *entity);

/* Makes the chosen of the first two combat choices less likely and the other more. */
void EntityAi_LowerChoiceWeight(Entity *entity, s32 chosen);

/* The turn command (0x100, 0x200) towards the target, 0 when it is within the turn limit. */
int EntityAi_TurnCommand(Entity *entity);

/* The move command towards the target: turn, up or down, and forward while farther than `range`. */
s32 EntityAi_MoveCommand3D(Entity *entity, s32 mark_far, f32 range);

/* The move command towards the target: turn, and forward while farther than `range`. */
s32 EntityAi_MoveCommand(Entity *entity, s32 mark_far, f32 range);

/* As EntityAi_MoveCommand, but moves forward only once it does not have to turn. */
s32 EntityAi_TurnOrMoveCommand(Entity *entity, s32 far_flag, f32 reach);

/* The move command for turning aside from a push, with up or down by the push. */
int EntityAi_PushTurnCommand3D(Entity *entity, int turn);

#endif
