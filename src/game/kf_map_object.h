/* Map objects: the per-map object tables, their setup, update, drawing and animation. */
#ifndef KF_MAP_OBJECT_H
#define KF_MAP_OBJECT_H

#include "common.h"
#include "kf_model.h"

extern s32 D_004467A8[2];
extern int D_004467B0[2];
extern s32 D_004467B8[2];

void MapObject_InitMap(int map);
void MapObject_StartMap(int map);
void MapObject_UnloadMap(int map);
void MapObject_SaveAllFlags(void);
void MapObject_OffsetAll(int map, f32 *offset);
void MapObject_UpdateAll(void);
int MapObject_UseItem(MapObject *object, int item);
void MapObject_Signal(MapObject *object, int signal);
void MapObject_DrawAll(int mode, int only_one, int which);
void MapObject_Activate(MapObject *object);
void MapObject_Deactivate(MapObject *object);
MapObject *MapObject_GetById(int map, int object_id);
void MapObject_Break(MapObject *object);
s16 MapObject_StartAnim(MapObject *object, int anim); // match: callers keep $2 free across the call
int MapObject_StepAnim(MapObject *object);
void MapObject_MarkEmptied(MapObject *object);
void MapObject_MarkEmptiedById(int map, int object_id);
f32 MapObject_GetTurnToward(MapObject *object, Vector4 *point);
void MapObject_SetScripted(MapObject *object, int scripted);
void MapObject_PlayScriptedAnim(MapObject *object, int anim, int bit22);
void MapObject_SetScriptedBit21(MapObject *object, int bit21);
void MapObject_ClearMap(int map);

#endif
