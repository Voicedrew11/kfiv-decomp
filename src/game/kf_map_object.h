/* Map objects: the per-map object tables, their setup, update, drawing and animation. */
#ifndef KF_MAP_OBJECT_H
#define KF_MAP_OBJECT_H

#include "common.h"
#include "kf_model.h"

extern s32 D_004467A8[2];
extern int D_004467B0[2];
extern s32 D_004467B8[2];

void KF_MapObject_InitMap(int map);
void KF_MapObject_StartMap(int map);
void KF_MapObject_UnloadMap(int map);
void KF_MapObject_SaveAllFlags(void);
void KF_MapObject_OffsetAll(int map, f32 *offset);
void KF_MapObject_UpdateAll(void);
int KF_MapObject_UseItem(MapObject *object, int item);
void KF_MapObject_Signal(MapObject *object, int signal);
void KF_MapObject_DrawAll(int mode, int only_one, int which);
void KF_MapObject_Activate(MapObject *object);
void KF_MapObject_Deactivate(MapObject *object);
MapObject *KF_MapObject_GetById(int map, int object_id);
void KF_MapObject_Break(MapObject *object);
s16 KF_MapObject_StartAnim(MapObject *object, int anim); // match: callers keep $2 free across the call
int KF_MapObject_StepAnim(MapObject *object);
void KF_MapObject_MarkEmptied(MapObject *object);
void KF_MapObject_MarkEmptiedById(int map, int object_id);
f32 KF_MapObject_GetTurnToward(MapObject *object, Vector4 *point);
void KF_MapObject_SetScripted(MapObject *object, int scripted);
void KF_MapObject_PlayScriptedAnim(MapObject *object, int anim, int bit22);
void KF_MapObject_SetScriptedBit21(MapObject *object, int bit21);
void KF_MapObject_ClearMap(int map);

#endif
