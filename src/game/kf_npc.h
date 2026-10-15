/* The talking characters: entity types 15 and 81. Every character a Recollection page describes has a chr.prm
   record of type 15 (from the game data). Type 15 turns to the player and watches it while scripts put it in
   states 4 to 6; type 81 only stands and walks back to its spawn point. */
#ifndef KF_NPC_H
#define KF_NPC_H

#include "common.h"
#include "kf_entity.h"

/* The set-up function of entity type 15, the talking characters. */
void Npc_Init(Entity *entity);

/* The think handler of type 15: turns to the player, watches it, turns back (states 4-6). */
void Npc_Think(Entity *entity, s32 unknown);

/* The set-up function of entity type 81: as Npc_Init, with Npc_ThinkType81 and Npc_OnHitType81. */
void Npc_InitType81(Entity *entity);

#endif
