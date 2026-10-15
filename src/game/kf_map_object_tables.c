/* Map object definitions, both slots' objects and per-kind handlers. */
#include "kf_game.h"
#include "kf_map.h"
#include "kf_model.h"

/* the handlers the kind tables name (kf_map_object_*.c) */
void MapObject_UpdateClosedChest(void *arg, int unused);
void MapObject_UpdateClosedDoor(MapObject *object);
void MapObject_UpdateOpenChest(MapObject *object);
void MapObject_UpdateSavePoint(MapObject *object);
void MapObject_MoveByOffset(MapObject *object, void *offset);
void MapObject_StartScenery(MapObject *object);
void MapObject_UpdateScenery(MapObject *object);
void MapObject_UpdateBreaking(MapObject *object);
void MapObject_StopAmbientSound(MapTrigger *trigger);
void MapObject_RebuildMatrices(char *arg);
int MapObject_RefuseItem();
void func_0011F228();
void func_0011F230();
void MapObject_StartDoor(MapObject *object);
void MapObject_UpdateOpeningDoor(MapObject *object);
void MapObject_UpdateOpenDoor(MapObject *object);
void MapObject_UpdateClosingDoor(MapObject *object);
void MapObject_SetDoorOpen(MapObject *object, int open);
void MapObject_RebuildMovingDoorMatrices(MapObject *object);
void MapObject_StartItemHolder(MapObject *object);
void MapObject_UpdateItemHolder(MapObject *object);
void MapObject_UpdateBreakingItemHolder(MapObject *object);
void MapObject_SetupItemHolder(MapObject *object);
void MapObject_StartChest(MapObject *object);
void MapObject_UpdateOpeningChest(MapObject *object);
void func_00120128();
void MapObject_OpenChestOnSignal(GameEvent *event, int mode);
void MapObject_SetupChest(MapObject *object);
void MapObject_StartDistanceFade(MapObject *object);
void MapObject_UpdateDistanceFade(MapObject *object);
void MapObject_RebuildBillboardMatrices(MapObject *object);
void MapObject_StartToggle(MapObject *object);
void MapObject_UpdateIdleToggle(MapObject *object);
void MapObject_UpdateMovingToggle(MapObject *object);
void MapObject_StartSwitch(MapObject *object);
void MapObject_UpdateOffSwitch(void *arg);
void MapObject_UpdateSwitchTurningOn(MapObject *object);
void MapObject_UpdateOnSwitch(void *arg);
void MapObject_UpdateSwitchTurningOff(MapObject *object);
void MapObject_SetSwitchOn(MapObject *object, int turn_on);
void MapObject_StartLight(MapObject *object);
void MapObject_UpdateLight(MapObject *object);
void MapObject_ReleaseLight(MapObject *object);
void MapObject_StartTouchTrap(MapObject *object);
void func_00120D88();
void MapObject_StartExaminable(MapObject *object);
void MapObject_UpdateExaminable(MapObject *object);
void MapObject_ShowExaminedSign(MapObject *object);
void MapObject_ShowExaminedMessage(MapObject *object);
void MapObject_WaitExaminedMessage(MapObject *object);
void MapObject_StartSymbolOfPassage(MapObject *object);
void func_00120ED8();
void MapObject_StartPedestal(MapObject *object);
void MapObject_UpdateEmptyPedestal(MapObject *object);
void MapObject_MovePedestalItemIn(MapObject *object);
void MapObject_SettlePedestalItem(MapObject *object);
void MapObject_UpdateFilledPedestal(MapObject *object);
void MapObject_LiftPedestalItem(MapObject *object);
void MapObject_MovePedestalItemOut(MapObject *object);
void MapObject_RebuildPedestalMatrices(char *object);
int MapObject_PlaceItemOnPedestal(MapObject *object, int item);
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
void MapObject_StartMarker(MapObject *object);
void MapObject_UpdateEmptyMarker(MapObject *object);
void MapObject_MoveMarkerStoneIn(MapObject *object);
void MapObject_SettleMarkerStone(MapObject *object);
void MapObject_UpdateFilledMarker(MapObject *object);
void MapObject_LiftMarkerStone(MapObject *object);
void MapObject_MoveMarkerStoneOut(MapObject *object);
void MapObject_RebuildMarkerMatrices(char *object);
int MapObject_PlaceStoneOnMarker(MapTrigger *trigger, int type_index);
void func_001226D0(MapObject *object);
void func_00122720(MapObject *object);
void func_00122730(MapObject *object);
void func_001227C0(MapObject *object);
void func_001227C8(MapObject *object);
void func_001227D0(MapObject *object);
void func_00122828(MapObject *object);
void func_00122868(MapObject *object);
void func_001228C0(MapObject *object);
void func_00122908(MapObject *object);
void func_00122960(MapTrigger *trigger, int turn_on);
void func_001229D0(MapTrigger *trigger);
void func_00122BE8(MapTrigger *trigger);
void func_00122CA8();
void func_00122CB0(MapTrigger *trigger);
void func_00122D60();
void func_00122D68(MapTrigger *trigger);
void func_00122E18(MapTrigger *trigger, int turn_on);
void func_00122E88(MapTrigger *trigger);
int func_00122F48(MapTrigger *trigger);

char map_object_defs[0x14000] = { 0 }; /* 0x38DA30 the param/obj.prm row of every object */

MapObject map_slot_objects[2][0x200] = { 0 }; /* 0x3A1A30 the objects of both map slots, a row each */
/* its second row, which other files name on its own */
asm(".globl D_003AFA30\nD_003AFA30 = map_slot_objects + 0xE000");  /* 0x3AFA30 the objects of the other loaded map */

void (*map_object_step_handlers[0x20][16])(MapObject *) = { /* 0x3BDA30 the step handler of each object kind */
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartItemHolder, MapObject_UpdateItemHolder, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreakingItemHolder,
    MapObject_StartChest, (void (*)(MapObject *))MapObject_UpdateClosedChest, MapObject_UpdateOpeningChest, MapObject_UpdateOpenChest, (void (*)(MapObject *))func_00120128, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDistanceFade, MapObject_UpdateDistanceFade, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartToggle, MapObject_UpdateIdleToggle, MapObject_UpdateMovingToggle, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartSwitch, (void (*)(MapObject *))MapObject_UpdateOffSwitch, MapObject_UpdateSwitchTurningOn, (void (*)(MapObject *))MapObject_UpdateOnSwitch, MapObject_UpdateSwitchTurningOff, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartLight, MapObject_UpdateLight, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartTouchTrap, (void (*)(MapObject *))func_00120D88, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartExaminable, MapObject_UpdateExaminable, MapObject_ShowExaminedSign, MapObject_ShowExaminedMessage, MapObject_WaitExaminedMessage, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartSymbolOfPassage, (void (*)(MapObject *))func_00120ED8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartPedestal, MapObject_UpdateEmptyPedestal, MapObject_MovePedestalItemIn, MapObject_SettlePedestalItem, MapObject_UpdateFilledPedestal, MapObject_LiftPedestalItem, MapObject_MovePedestalItemOut, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))MapObject_StartSinkingPlatform, MapObject_UpdateSinkingPlatform, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartLadder, MapObject_UpdateLadder, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartShooter, MapObject_UpdateShooter, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartPitfall, MapObject_UpdateClosedPitfall, MapObject_UpdateOpeningPitfall, MapObject_UpdateOpenPitfall, MapObject_UpdateClosingPitfall, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartMarker, MapObject_UpdateEmptyMarker, MapObject_MoveMarkerStoneIn, MapObject_SettleMarkerStone, MapObject_UpdateFilledMarker, MapObject_LiftMarkerStone, MapObject_MoveMarkerStoneOut, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    func_001226D0, MapObject_UpdateSavePoint, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    func_00122720, func_00122730, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_001227C0,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))func_00122E88, (void (*)(MapObject *))func_00122F48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    func_001227D0, func_00122828, func_00122868, func_001228C0, func_00122908, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))func_00122BE8, (void (*)(MapObject *))func_00122CA8, (void (*)(MapObject *))func_00122CB0, (void (*)(MapObject *))func_00122D60, (void (*)(MapObject *))func_00122D68, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
};

void (*map_object_draw_hooks[0x20])(MapObject *) = { /* 0x3BE230 the kind's draw hook */
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, MapObject_ReleaseLight, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
    (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound, (void (*)(MapObject *))MapObject_StopAmbientSound,
};

int (*map_object_item_hooks[0x20])(MapObject *, int) = { /* 0x3BE2B0 the kind's item hook */
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, MapObject_PlaceItemOnPedestal, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_PlaceStoneOnMarker,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
    (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_RefuseItem,
};

void (*map_object_signal_hooks[0x20])(MapObject *, int) = { /* 0x3BE330 the kind's signal hook */
    (void (*)(MapObject *, int))func_0011F228, MapObject_SetDoorOpen, MapObject_SetDoorOpen, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))MapObject_OpenChestOnSignal, (void (*)(MapObject *, int))func_0011F228, MapObject_SetDoorOpen, (void (*)(MapObject *, int))func_0011F228,
    MapObject_SetSwitchOn, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    MapObject_SetShooterOn, MapObject_SetDoorOpen, MapObject_SetPitfallOpen, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_00122960, (void (*)(MapObject *, int))func_00122E18,
};

void (*map_object_update_hooks[0x20])(MapObject *) = { /* 0x3BE3B0 the kind's every-frame update */
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, MapObject_RebuildBillboardMatrices, MapObject_RebuildMovingDoorMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildPedestalMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMarkerMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))func_001229D0, (void (*)(MapObject *))MapObject_RebuildMatrices,
};

void (*map_object_offset_hooks[0x20])(MapObject *, f32 *) = { /* 0x3BE430 the kind's move by an offset */
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
    (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset, (void (*)(MapObject *, f32 *))MapObject_MoveByOffset,
};

void (*map_object_setup_hooks[0x20])(MapObject *) = { /* 0x3BE4B0 the kind's set-up */
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, MapObject_SetupItemHolder,
    MapObject_SetupChest, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, func_001227C8, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
};
