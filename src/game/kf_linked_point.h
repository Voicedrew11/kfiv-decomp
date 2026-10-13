#ifndef KF_LINKED_POINT_H
#define KF_LINKED_POINT_H

#include "common.h"
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_model.h"

void KF_LinkedPoint_Init(void);
int KF_LinkedPoint_Start(f32 duration, void *start, void *end, int kind_a, int kind_b, u32 list);
int KF_LinkedPoint_Allocate(f32 duration, void *start, void *end, u8 kind_a, int kind_b, int owner, u32 list);
int KF_LinkedPoint_StartDefault(f32 duration, void *start, void *end);
void KF_LinkedPoint_ReleaseIndex(u32 index);
void KF_LinkedPoint_ReleaseByKind(int kind);
void KF_LinkedPoint_FindHitsForModel(ModelInstance *model);
void KF_LinkedPoint_FindHitsExclude(ModelInstance *model, int handle);
void KF_LinkedPoint_FindHitsForOwner(ModelInstance *model, int owner);
void KF_LinkedPoint_FindHitsForMapDraw(MapDrawRecord *record, ModelInstance *model);
void KF_LinkedPoint_UpdateFollows(void);
void KF_LinkedPoint_TransformFollows(void *transform);
void KF_LinkedPoint_Retarget(f32 duration, s32 index, void *target);

#endif
