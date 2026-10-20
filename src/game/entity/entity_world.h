#ifndef KF_ENTITY_WORLD_H
#define KF_ENTITY_WORLD_H

#include "entity/entity.h"

void Entity_InitFromMapSpawns(int map);
void Entity_ClearFlag10InSlot(int array_index);
void Entity_ResetToSpawn(Entity *entity);
void Entity_ClearMapSlot(int array_index);
void Entity_SetSpawnedFlags(Entity *entity);
void Entity_OffsetAllInSlot(int array_index, f32 *offset);
void Entity_UpdateAll(void);
void Entity_DrawAll(int list);
Entity *Entity_GetBySpawnId(int entity_id);
Entity *Entity_GetBySpawnIdForMap(int map_index, int entity_id);

/* entity/entity_world.c */
void Entity_UpdateInert(Entity *entity);
void Entity_InitInert(Entity *entity);
void Entity_StartupHook(void);

/* data still in asm */
extern Vector4Arg entity_unit_scale;
extern const f32 entity_no_hit_distance[];

#endif
