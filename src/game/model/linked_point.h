#ifndef KF_LINKED_POINT_H
#define KF_LINKED_POINT_H

#include "common.h"
#include "entity/entity.h"
#include "map/map.h"
#include "model/model.h"

void LinkedPoint_Init(void);
int LinkedPoint_Start(f32 duration, void *start, void *end, int kind_a, int kind_b, u32 list);
int LinkedPoint_Allocate(f32 duration, void *start, void *end, u8 kind_a, int kind_b, int owner, u32 list);
int LinkedPoint_StartDefault(f32 duration, void *start, void *end);
void LinkedPoint_ReleaseIndex(u32 index);
void LinkedPoint_ReleaseByKind(int kind);
void LinkedPoint_FindHitsForModel(ModelInstance *model);
void LinkedPoint_FindHitsExclude(ModelInstance *model, int handle);
void LinkedPoint_FindHitsForOwner(ModelInstance *model, int owner);
void LinkedPoint_FindHitsForMapDraw(MapDrawRecord *record, ModelInstance *model);
void LinkedPoint_UpdateFollows(void);
void LinkedPoint_TransformFollows(void *transform);
void LinkedPoint_Retarget(f32 duration, s32 index, void *target);

#endif
