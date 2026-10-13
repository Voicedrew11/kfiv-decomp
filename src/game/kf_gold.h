#ifndef KF_GOLD_H
#define KF_GOLD_H

#include "common.h"
#include "kf_entity.h"
#include "kf_map.h"

void Gold_Init(void);
void Gold_InitMap(int map);
void Gold_UnlinkMap(int slot);
void Gold_MoveMap(int slot, f32 *offset);
void Gold_Update(void);
void Gold_Draw(s32 list);
int Gold_Drop(Vector4 *position, Vector4 *rotation, int map, int amount, int object_id, int on_ground,
                 int other_map);
int Gold_DropFromEntity(Entity *entity);

#endif
