/* Map objects: the per-map object tables, their setup, update, drawing and animation. */
#ifndef KF_MAP_OBJECT_H
#define KF_MAP_OBJECT_H

#include "common.h"
#include "model/model.h"
#include "core/game.h"
#include "map/map.h"

/* the kinds of map object (MapObjectUnk04.kind) the code names; param/obj.prm gives the others' names */
#define MAP_OBJECT_KIND_DOOR                1
#define MAP_OBJECT_KIND_DOUBLE_DOOR         2       /* its collision clears once both leaves stand 225 apart */
#define MAP_OBJECT_KIND_ITEM_HOLDER         3       /* bones and gravestones that hold an item */
#define MAP_OBJECT_KIND_CHEST               4
#define MAP_OBJECT_KIND_SYMBOL_OF_PASSAGE   12      /* where the wands take the player (Player_UseItem) */
#define MAP_OBJECT_KIND_CONTAINER           21      /* barrels, pots and wooden boxes */
#define MAP_OBJECT_KIND_AMBIENT             0       /* scenery that can play an ambient sound */
#define MAP_OBJECT_KIND_DISTANCE_FADE       5       /* fades with the distance to the player */
#define MAP_OBJECT_KIND_MOVING_DOOR         6       /* a door that moves with its second part */
#define MAP_OBJECT_KIND_LIGHT               9       /* a light at a point of the model */
#define MAP_OBJECT_KIND_SIGNAL_DOOR         0x11    /* a door between the shooter and pitfall kinds, handled as kind 1 */
#define MAP_OBJECT_KIND_PITFALL             0x12    /* a pit that opens under the player (mechanism.c) */

/* MapObjectBytes.step and anim values every kind shares; each kind's own steps are named in its file */
#define MAP_OBJECT_STEP_NOT_STARTED         0       /* the kind's state machine is not started */
#define MAP_OBJECT_STEP_BROKEN              0xF     /* MapObject_Break: breaking, its animation and effect */
#define MAP_OBJECT_ANIM_NONE                -1      /* no animation started yet */
#define MAP_OBJECT_ANIM_FIRST               0       /* the definition's first animation */
#define MAP_OBJECT_ANIM_BREAK               1       /* the breaking animation */
#define MAP_OBJECT_ANIM_OFF                 4       /* closed, off or unlit */
#define MAP_OBJECT_ANIM_ON                  5       /* open, on or lit */

extern s32 map_objects_started[2];
extern int map_object_placements[2];
extern s32 map_object_counts[2];

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

/* Units a moved map object covers per frame (pedestal and socket moves). */
#define MAP_OBJECT_MOVE_SPEED 6.6666665f

/* map_object/chest.c */
void MapObject_UpdateOpenChest(MapObject *object);
void MapObject_UpdateClosedChest(MapObject *object);
void MapObject_SetupItemHolder(MapObject *object);
void MapObject_UpdateItemHolder(MapObject *object);
void MapObject_UpdateBreakingItemHolder(MapObject *object);
void MapObject_SetupChest(MapObject *object);
void MapObject_StartChest(MapObject *object);
void MapObject_UpdateOpeningChest(MapObject *object);
void MapObject_ChestStep4Empty(MapObject *object);
void MapObject_OpenChestOnSignal(GameEvent *event, int mode);
void MapObject_SpawnHeldItem(MapObject *object);

/* map_object/door.c */
void MapObject_UpdateOpeningDoor(MapObject *object);
void MapObject_StartDoor(MapObject *object);
void MapObject_UpdateClosedDoor(MapObject *object);
void MapObject_UpdateOpenDoor(MapObject *object);
void MapObject_UpdateClosingDoor(MapObject *object);
void MapObject_SetDoorOpen(MapObject *object, int open);
void MapObject_RebuildMovingDoorMatrices(MapObject *object);
void MapObject_StartItemHolder(MapObject *object);

/* map_object/fade.c */
void MapObject_StartDistanceFade(MapObject *object);
void MapObject_UpdateDistanceFade(MapObject *object);
void MapObject_RebuildBillboardMatrices(MapObject *object);

/* map_object/light.c */
void MapObject_StartLight(MapObject *object);
void MapObject_UpdateLight(MapObject *object);
void MapObject_ReleaseLight(MapObject *object);
void MapObject_StartStatusOnTouch(MapObject *object);
void MapObject_IdleStatusOnTouch(MapObject *object);

/* map_object/map_object.c */
void MapObject_ClearAllMaps(void);
void MapObject_DrawModel(ModelInstance *model);
void MapObject_DrawModelWithAlpha(ModelInstance *model);
void MapObject_DrawModelNoDepthWrite(ModelInstance *model);
void MapObject_UpdateNoBonePoints(ModelInstance *model);
void *MapObject_GetPositionAsPoint(void *model, int index);
void MapObject_StopSound(MapObject *object);

/* map_object/map_object_common.c */
void MapObject_MoveByOffset(MapObject *object, void *offset);
void MapObject_StartScenery(MapObject *object);
void MapObject_UpdateScenery(MapObject *object);
void MapObject_UpdateBreaking(MapObject *object);
void MapObject_StopAmbientSound(MapTrigger *trigger);
void MapObject_RebuildMatrices(MapObject *object);
int MapObject_RefuseItem(MapObject *object, int item);
void MapObject_IgnoreSignal(MapObject *object, int turn_on);
void MapObject_SkipSetUp(MapObject *object);

/* map_object/map_object_tables.c */
extern char map_object_defs[];
extern MapObject map_slot_objects[][0x200];
extern MapObject map_slot1_objects[];

/* map_object/map_trigger.c */
void MapObject_FadeInObject(MapTrigger *trigger);
void MapObject_StartSavePoint(MapObject *object);
void MapObject_UpdateSavePoint(MapObject *object);
void MapObject_StartContainer(MapObject *object);
void MapObject_UpdateContainer(MapObject *object);
void MapObject_UpdateBreakingContainer(MapObject *object);
void MapObject_SetUpContainer(MapObject *object);
void MapObject_StartPivotSwitch(MapObject *object);
void MapObject_UpdatePivotSwitchOff(MapObject *object);
void MapObject_TurnPivotSwitchOn(MapObject *object);
void MapObject_UpdatePivotSwitchOn(MapObject *object);
void MapObject_TurnPivotSwitchOff(MapObject *object);
void MapObject_SignalPivotSwitch(MapTrigger *trigger, int turn_on);
void MapObject_PosePivotSwitch(MapTrigger *trigger);
void MapObject_StartFadeObject(MapTrigger *trigger);
void MapObject_SignalFadeObject(MapTrigger *trigger, int turn_on);
void MapObject_KeepFadeObjectHidden(MapObject *object);
void MapObject_KeepFadeObjectShown(MapObject *object);
void MapObject_FadeOutObject(MapTrigger *trigger);
void MapObject_StartPulse(MapTrigger *trigger);
int MapObject_UpdatePulse(MapTrigger *trigger);
void MapObject_ShowKeyUsed(int item);

/* map_object/mechanism.c */
void MapObject_StartSinkingPlatform(MapTrigger *trigger);
void MapObject_UpdateSinkingPlatform(MapObject *object);
void MapObject_StartLadder(MapObject *object);
void MapObject_UpdateLadder(MapObject *object);
void MapObject_StartShooter(MapObject *object);
void MapObject_UpdateShooter(MapObject *object);
void MapObject_SetShooterOn(MapObject *object, int enabled);
void MapObject_StartPitfall(MapObject *object);
void MapObject_UpdateClosedPitfall(MapObject *object);
void MapObject_UpdateOpeningPitfall(MapObject *object);
void MapObject_UpdateOpenPitfall(MapObject *object);
void MapObject_UpdateClosingPitfall(MapObject *object);
void MapObject_SetPitfallOpen(MapObject *object, int turn_on);

/* map_object/pedestal.c */
void MapObject_RebuildMarkerMatrices(MapObject *object);
void MapObject_StartMarker(MapObject *object);
int MapObject_PlaceStoneOnMarker(MapTrigger *trigger, int type_index);
void MapObject_UpdateEmptyMarker(MapObject *object);
void MapObject_MoveMarkerStoneIn(MapObject *object);
void MapObject_SettleMarkerStone(MapObject *object);
void MapObject_UpdateFilledMarker(MapObject *object);
void MapObject_LiftMarkerStone(MapObject *object);
void MapObject_MoveMarkerStoneOut(MapObject *object);
void MapObject_ShowMarkerStone(MapObject *object, int item);

/* map_object/sign.c */
void MapObject_StartExaminable(MapObject *object);
void MapObject_UpdateExaminable(MapObject *object);
void MapObject_ShowExaminedSign(MapObject *object);
void MapObject_ShowExaminedMessage(MapObject *object);
void MapObject_WaitExaminedMessage(MapObject *object);
void MapObject_StartSymbolOfPassage(MapObject *object);
void MapObject_IdleSymbolOfPassage(MapObject *object);

/* map_object/socket.c */
void MapObject_StartPedestal(MapObject *object);
int MapObject_PlaceItemOnPedestal(MapObject *object, int item);
void MapObject_UpdateEmptyPedestal(MapObject *object);
void MapObject_MovePedestalItemIn(MapObject *object);
void MapObject_SettlePedestalItem(MapObject *object);
void MapObject_UpdateFilledPedestal(MapObject *object);
void MapObject_LiftPedestalItem(MapObject *object);
void MapObject_MovePedestalItemOut(MapObject *object);
void MapObject_RebuildPedestalMatrices(MapObject *object);

/* map_object/switch.c */
void MapObject_UpdateOffSwitch(MapObject *switch_object);
void MapObject_UpdateOnSwitch(MapObject *switch_object);
void MapObject_StartToggle(MapObject *object);
void MapObject_UpdateIdleToggle(MapObject *object);
void MapObject_UpdateMovingToggle(MapObject *object);
void MapObject_StartSwitch(MapObject *object);
void MapObject_UpdateSwitchTurningOn(MapObject *object);
void MapObject_UpdateSwitchTurningOff(MapObject *object);
void MapObject_SetSwitchOn(MapObject *object, int turn_on);

/* data still in asm */
extern void *faced_object[];
extern Qword object_item_scale;

/* data still in asm */
extern EventRunner view_ray_status;

#endif
