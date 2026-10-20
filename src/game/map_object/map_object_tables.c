/* Map object definitions, both slots' objects and per-kind handlers. */
#include "core/game.h"
#include "map/map.h"
#include "map_object/map_object.h"
#include "model/model.h"

char map_object_defs[0x14000] = { 0 }; /* 0x38DA30 the param/obj.prm row of every object */

MapObject map_slot_objects[2][0x200] = { 0 }; /* 0x3A1A30 the objects of both map slots, a row each */
/* its second row, which other files name on its own */
asm(".globl map_slot1_objects\nmap_slot1_objects = map_slot_objects + 0xE000");  /* 0x3AFA30 the objects of map slot 1 */

void (*map_object_step_handlers[0x20][16])(MapObject *) = { /* 0x3BDA30 the step handler of each object kind */
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartItemHolder, MapObject_UpdateItemHolder, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreakingItemHolder,
    MapObject_StartChest, (void (*)(MapObject *))MapObject_UpdateClosedChest, MapObject_UpdateOpeningChest, MapObject_UpdateOpenChest, MapObject_ChestStep4Empty, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDistanceFade, MapObject_UpdateDistanceFade, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartToggle, MapObject_UpdateIdleToggle, MapObject_UpdateMovingToggle, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartSwitch, MapObject_UpdateOffSwitch, MapObject_UpdateSwitchTurningOn, MapObject_UpdateOnSwitch, MapObject_UpdateSwitchTurningOff, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartLight, MapObject_UpdateLight, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartStatusOnTouch, MapObject_IdleStatusOnTouch, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartExaminable, MapObject_UpdateExaminable, MapObject_ShowExaminedSign, MapObject_ShowExaminedMessage, MapObject_WaitExaminedMessage, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartSymbolOfPassage, MapObject_IdleSymbolOfPassage, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartPedestal, MapObject_UpdateEmptyPedestal, MapObject_MovePedestalItemIn, MapObject_SettlePedestalItem, MapObject_UpdateFilledPedestal, MapObject_LiftPedestalItem, MapObject_MovePedestalItemOut, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))MapObject_StartSinkingPlatform, MapObject_UpdateSinkingPlatform, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartLadder, MapObject_UpdateLadder, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartShooter, MapObject_UpdateShooter, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartDoor, MapObject_UpdateClosedDoor, MapObject_UpdateOpeningDoor, MapObject_UpdateOpenDoor, MapObject_UpdateClosingDoor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartPitfall, MapObject_UpdateClosedPitfall, MapObject_UpdateOpeningPitfall, MapObject_UpdateOpenPitfall, MapObject_UpdateClosingPitfall, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartMarker, MapObject_UpdateEmptyMarker, MapObject_MoveMarkerStoneIn, MapObject_SettleMarkerStone, MapObject_UpdateFilledMarker, MapObject_LiftMarkerStone, MapObject_MoveMarkerStoneOut, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartSavePoint, MapObject_UpdateSavePoint, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartContainer, MapObject_UpdateContainer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreakingContainer,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartScenery, MapObject_UpdateScenery, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))MapObject_StartPulse, (void (*)(MapObject *))MapObject_UpdatePulse, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    MapObject_StartPivotSwitch, MapObject_UpdatePivotSwitchOff, MapObject_TurnPivotSwitchOn, MapObject_UpdatePivotSwitchOn, MapObject_TurnPivotSwitchOff, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
    (void (*)(MapObject *))MapObject_StartFadeObject, MapObject_KeepFadeObjectHidden, (void (*)(MapObject *))MapObject_FadeInObject, MapObject_KeepFadeObjectShown, (void (*)(MapObject *))MapObject_FadeOutObject, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_UpdateBreaking,
};

void (*map_object_end_hooks[0x20])(MapTrigger *) = { /* 0x3BE230 the kind's end hook, run by MapObject_Deactivate */
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, (void (*)(MapTrigger *))MapObject_ReleaseLight, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
    MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound, MapObject_StopAmbientSound,
};

int (*map_object_item_hooks[0x20])(MapObject *, int) = { /* 0x3BE2B0 the kind's item hook */
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_PlaceItemOnPedestal, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, (int (*)(MapObject *, int))MapObject_PlaceStoneOnMarker,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
    MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem, MapObject_RefuseItem,
};

void (*map_object_signal_hooks[0x20])(MapObject *, int) = { /* 0x3BE330 the kind's signal hook */
    MapObject_IgnoreSignal, MapObject_SetDoorOpen, MapObject_SetDoorOpen, MapObject_IgnoreSignal,
    (void (*)(MapObject *, int))MapObject_OpenChestOnSignal, MapObject_IgnoreSignal, MapObject_SetDoorOpen, MapObject_IgnoreSignal,
    MapObject_SetSwitchOn, MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal,
    MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal,
    MapObject_SetShooterOn, MapObject_SetDoorOpen, MapObject_SetPitfallOpen, MapObject_IgnoreSignal,
    MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal,
    MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal, MapObject_IgnoreSignal,
    MapObject_IgnoreSignal, MapObject_IgnoreSignal, (void (*)(MapObject *, int))MapObject_SignalPivotSwitch, (void (*)(MapObject *, int))MapObject_SignalFadeObject,
};

void (*map_object_update_hooks[0x20])(MapObject *) = { /* 0x3BE3B0 the kind's every-frame update */
    MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildBillboardMatrices, MapObject_RebuildMovingDoorMatrices, MapObject_RebuildMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildPedestalMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMarkerMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices,
    MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices, MapObject_RebuildMatrices,
    (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_RebuildMatrices, (void (*)(MapObject *))MapObject_PosePivotSwitch, (void (*)(MapObject *))MapObject_RebuildMatrices,
};

void (*map_object_offset_hooks[0x20])(MapObject *, void *) = { /* 0x3BE430 the kind's move by an offset */
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
    MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset, MapObject_MoveByOffset,
};

void (*map_object_setup_hooks[0x20])(MapObject *) = { /* 0x3BE4B0 the kind's set-up */
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SetupItemHolder,
    MapObject_SetupChest, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SetUpContainer, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
    MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp, MapObject_SkipSetUp,
};
