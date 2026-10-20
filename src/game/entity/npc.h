/* The talking characters: entity types 15 and 81. Every character a Recollection page describes has a chr.prm
   record of type 15 (from the game data). Type 15 turns to the player and watches it while scripts put it in
   states 4 to 6; type 81 only stands and walks back to its spawn point. */
#ifndef KF_NPC_H
#define KF_NPC_H

#include "common.h"
#include "entity/entity.h"

/* Entity.state of type 15, set by scripts (states 0 to 3 stand) */
#define NPC_STATE_TURN_TO_PLAYER    4       /* turns to the player, or reacts to it */
#define NPC_STATE_WATCH_PLAYER      5       /* watches the player until it is farther than EntityParams.attack_range */
#define NPC_STATE_TURN_BACK         6       /* turns back to the spawn heading, or stops watching */

/* The set-up function of entity type 15, the talking characters. */
void Npc_Init(Entity *entity);

/* The think handler of type 15: turns to the player, watches it, turns back (states 4-6). */
void Npc_Think(Entity *entity, s32 run_now);

/* The set-up function of entity type 81: as Npc_Init, with Npc_ThinkType81 and Npc_OnHitType81. */
void Npc_InitType81(Entity *entity);

#endif
