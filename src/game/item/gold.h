#ifndef KF_GOLD_H
#define KF_GOLD_H

#include "common.h"
#include "entity/entity.h"
#include "map/map.h"

void Gold_Init(void);
void Gold_InitMap(int map);
void Gold_UnlinkMap(int slot);
void Gold_MoveMap(int slot, f32 *offset);
void Gold_Update(void);
void Gold_Draw(s32 list);
int Gold_Drop(Vector4 *position, Vector4 *rotation, int map, int amount, int object_id, int on_ground,
                 int other_map);
int Gold_DropFromEntity(Entity *entity);

/* data still in asm */
extern f32 gold_drop_unmeasured_distance[];
extern MapActor *map_actors_in_use;
extern s32 map_actor_drop_counts[2];

/* item/gold.c */
void Gold_PrepareFadedDraw(OverlaySprite *sprite);
MapActor *Gold_FindFree(int slot);
MapActor *Gold_FindOldest(int slot);
void Gold_LinkInUse(MapActor *actor);
MapActor *Gold_UnlinkFromList(MapActor *actor);
void Gold_SaveMark(MapActor *actor);
void Gold_ClearMark(MapActor *actor);

#endif
