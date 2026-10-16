/* effect_kind_handlers: the start and update handler of every kind of effect, the row EffectDef.kind_index picks
   (Effect_SpawnFromEffect and Effect_UpdateAll call them); only the first 128 rows are set. */
#include "common.h"
#include "effect/effect.h"

/* Prototypes effect/effect.h lacks for the handlers below; effect/effect_kinds_a.c and effect/fx_kinds.c define them. */
void EffectAimAtPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectAimAtPlayer_Update(EffectActor *actor);
void EffectAimedChase_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectAimedChase_Update(EffectActor *actor);
void EffectAimedHoming_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectAimedHoming_Update(EffectActor *actor);
void EffectArc_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectArc_Update(EffectActor *actor);
void EffectAtTargetFloor_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectAtTargetFloor_Update(EffectActor *effect);
void EffectAtTarget_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectAtTarget_Update(EffectActor *effect);
void EffectBeamStretch_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBeamStretch_Update(EffectActor *effect);
void EffectBeam_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectBeam_Update(EffectActor *effect);
void EffectBobFly_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBobFly_Update(EffectActor *effect);
void EffectBobHoming_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectBobHoming_Update(EffectActor *effect);
void EffectBolt_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectBolt_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_StartCounted(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_StartWithTargeters(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_Update(EffectActor *actor);
void EffectBoneFade_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBoneFade_Update(EffectActor *actor);
void EffectBowDraw_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBowDraw_Update(EffectActor *actor);
void EffectChain_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectChain_Update(EffectActor *effect);
void EffectChase_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectChase_Update(EffectActor *actor);
void EffectConverge_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectConverge_StartInSpread(EffectActor *effect, EffectSlot *spawn_slot);
void EffectConverge_Update(EffectActor *effect);
void EffectCorkscrew_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectCorkscrew_Update(EffectActor *effect);
void EffectDelayedShot_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectDelayedShot_Update(EffectActor *effect);
void EffectDistanceFade_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectDistanceFade_Update(EffectActor *effect);
void EffectDrop_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectDrop_Update(EffectActor *effect);
void EffectEmitter_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectEmitter_Update(EffectActor *effect);
void EffectFallToFloor_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFallToFloor_Update(EffectActor *actor);
void EffectFall_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFall_Update(EffectActor *effect);
void EffectFan_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFan_Update(EffectActor *effect);
void EffectFloorChase_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFloorChase_Update(EffectActor *actor);
void EffectFloorDelay_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFloorDelay_Update(EffectActor *effect);
void EffectFloorSkim_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFloorSkim_Update(EffectActor *effect);
void EffectFly_StartGrowing(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFly_StartRaised(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFly_StartWithTargeters(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFly_Update(EffectActor *effect);
void EffectFly_UpdateGrowing(EffectActor *effect);
void EffectFollowLight_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowLight_Update(EffectActor *effect);
void EffectFollowPoseScaled_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowPoseScaled_Update(EffectActor *effect);
void EffectFollowPose_Start(EffectActor *effect);
void EffectFollowPose_Update(EffectActor *effect);
void EffectFollowPosition_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowPosition_Update(EffectActor *effect);
void EffectFollow_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollow_Update(EffectActor *effect);
void EffectGroundChase_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectGroundChase_Update(EffectActor *actor);
void EffectGroundFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundFan_Update(EffectActor *actor);
void EffectGroundRunWait_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundRunWait_Update(EffectActor *actor);
void EffectGroundRun_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundRun_Update(EffectActor *effect);
void EffectGroundSway_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundSway_Update(EffectActor *effect);
void EffectGroundTrail_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundTrail_Update(EffectActor *effect);
void EffectHandover_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHandover_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHandover_Update(EffectActor *effect);
void EffectHitWindow_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHitWindow_Update(EffectActor *effect);
void EffectHomeOnEntity_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHomeOnEntity_Update(EffectActor *actor);
void EffectHomeOnPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHomeOnPlayer_Update(EffectActor *effect);
void EffectHomingChain_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHomingChain_Update(EffectActor *effect);
void EffectHoverShot_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHoverShot_Update(EffectActor *effect);
void EffectIdle_Start(void);
void EffectIdle_Update(EffectActor *effect);
void EffectImpactLob_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectImpactLob_Update(EffectActor *actor);
void EffectImpactShot_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectImpactShot_Update(EffectActor *effect);
void EffectInheritSpeed_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectInheritSpeed_Update(EffectActor *effect);
void EffectJitterOrbit_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectJitterOrbit_Update(EffectActor *effect);
void EffectJitterWobble_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectJitterWobble_Update(EffectActor *effect);
void EffectLateFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLateFall_StartWithTargeters(EffectActor *actor, EffectSlot *spawn_slot);
void EffectLateFall_Update(EffectActor *actor);
void EffectLaunchFromOwner_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLaunchFromOwner_Update(EffectActor *effect);
void EffectLead_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLead_Update(EffectActor *effect);
void EffectLevelFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLevelFan_Update(EffectActor *effect);
void EffectLevelHeight_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectLevelHeight_Update(EffectActor *effect);
void EffectLight_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLight_Update(EffectActor *effect);
void EffectLingerShot_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectLingerShot_Update(EffectActor *actor);
void EffectLob_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLob_Update(EffectActor *effect);
void EffectLoopUntilFlag_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLoopUntilFlag_Update(EffectActor *effect);
void EffectLoop_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLoop_UpdateRandomRoll(EffectActor *effect);
void EffectOnFloor_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectOnFloor_Update(EffectActor *effect);
void EffectOwnerFloor_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectOwnerFloor_Update(EffectActor *effect);
void EffectPickTargets_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectPickTargets_Update(EffectActor *effect);
void EffectPitchScale_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectPitchScale_Update(EffectActor *effect);
void EffectProbedFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectProbedFall_Update(EffectActor *effect);
void EffectRay_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectRay_StartAtPlayer(EffectActor *effect, EffectSlot *spawn_slot);
void EffectRay_Update(EffectActor *effect);
void EffectRepeater_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectRepeater_Update(EffectActor *effect);
void EffectScatterAroundPlayer_Start(EffectActor *effect);
void EffectScatterAroundPlayer_Update(EffectActor *effect);
void EffectSeekPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSeekPlayer_Update(EffectActor *actor);
void EffectSeekPoint_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSeekPoint_Update(EffectActor *effect);
void EffectShot_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectShot_Update(EffectActor *effect);
void EffectSound_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSpiral_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSpiral_Update(EffectActor *effect);
void EffectSplitShot_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSplitShot_Update(EffectActor *effect);
void EffectSpray_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSpray_Update(EffectActor *effect);
void EffectSurroundPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSurroundPlayer_Update(EffectActor *effect);
void EffectSweep_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSweep_Update(EffectActor *effect);
void EffectTiltFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTiltFall_Update(EffectActor *effect);
void EffectTrailHoming_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTrailHoming_Update(EffectActor *actor);
void EffectTrail_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTrail_Update(EffectActor *actor);
void EffectTurnFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTurnFan_Update(EffectActor *effect);
void EffectWander_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectWander_Update(EffectActor *effect);
void EffectWeaponSound_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectWhileGroundFan_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectWhileGroundFan_Update(EffectActor *effect);
void EffectWobble_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectWobble_Update(EffectActor *effect);

void (*effect_kind_handlers[256][2])() = { /* 0x402550 */
    { EffectFly_Start, EffectFly_Update },
    { EffectFly_StartRaised, EffectFly_Update },
    { EffectAtTarget_Start, EffectAtTarget_Update },
    { EffectAtTargetFloor_Start, EffectAtTargetFloor_Update },
    { EffectLoop_Start, EffectLoop_Update },
    { EffectEmitter_Start, EffectEmitter_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectWhileGroundFan_Start, EffectWhileGroundFan_Update },
    { EffectGroundChase_Start, EffectGroundChase_Update },
    { EffectGroundRunWait_Start, EffectGroundRunWait_Update },
    { EffectLob_Start, EffectLob_Update },
    { EffectFall_Start, EffectFall_Update },
    { EffectGroundTrail_Start, EffectGroundTrail_Update },
    { EffectChase_Start, EffectChase_Update },
    { EffectGroundRun_Start, EffectGroundRun_Update },
    { EffectFloorSkim_Start, EffectFloorSkim_Update },
    { EffectGroundSway_Start, EffectGroundSway_Update },
    { EffectOnFloor_Start, EffectOnFloor_Update },
    { EffectArc_Start, EffectArc_Update },
    { EffectLead_Start, EffectLead_Update },
    { EffectFollow_Start, EffectFollow_Update },
    { EffectBeam_Start, EffectBeam_Update },
    { EffectHomingChain_Start, EffectHomingChain_Update },
    { EffectFan_Start, EffectFan_Update },
    { EffectBobHoming_Start, EffectBobHoming_Update },
    { EffectHoverShot_Start, EffectHoverShot_Update },
    { EffectTrail_Start, EffectTrail_Update },
    { EffectSplitShot_Start, EffectSplitShot_Update },
    { EffectPickTargets_Start, EffectPickTargets_Update },
    { EffectHandover_StartBySurface, EffectHandover_Update },
    { EffectBolt_StartBySurface, EffectBolt_Update },
    { EffectLingerShot_Start, EffectLingerShot_Update },
    { EffectAimedChase_Start, EffectAimedChase_Update },
    { EffectSweep_Start, EffectSweep_Update },
    { EffectImpactLob_Start, EffectImpactLob_Update },
    { EffectGroundFan_Start, EffectGroundFan_Update },
    { EffectLevelHeight_Start, EffectLevelHeight_Update },
    { EffectBobFly_Start, EffectBobFly_Update },
    { EffectBolt_StartWithTargeters, EffectBolt_Update },
    { EffectBolt_StartCounted, EffectBolt_Update },
    { EffectFollowPose_Start, EffectFollowPose_Update },
    { EffectBolt_Start, EffectBolt_Update },
    { EffectFly_StartWithTargeters, EffectFly_Update },
    { EffectHitWindow_Start, EffectHitWindow_Update },
    { EffectFollowPosition_Start, EffectFollowPosition_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectHandover_Start, EffectHandover_Update },
    { EffectConverge_Start, EffectConverge_Update },
    { EffectLoop_Start, EffectLoop_UpdateRandomRoll },
    { EffectFly_StartGrowing, EffectFly_UpdateGrowing },
    { EffectBoneFade_Start, EffectBoneFade_Update },
    { EffectLateFall_Start, EffectLateFall_Update },
    { EffectFloorChase_Start, EffectFloorChase_Update },
    { EffectSpray_Start, EffectSpray_Update },
    { EffectLateFall_StartWithTargeters, EffectLateFall_Update },
    { EffectFallToFloor_Start, EffectFallToFloor_Update },
    { EffectHomeOnPlayer_Start, EffectHomeOnPlayer_Update },
    { EffectTiltFall_Start, EffectTiltFall_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectFloorDelay_Start, EffectFloorDelay_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectTurnFan_Start, EffectTurnFan_Update },
    { EffectDelayedShot_Start, EffectDelayedShot_Update },
    { EffectLevelFan_Start, EffectLevelFan_Update },
    { EffectRay_StartAtPlayer, EffectRay_Update },
    { EffectBeamStretch_Start, EffectBeamStretch_Update },
    { EffectWobble_Start, EffectWobble_Update },
    { EffectProbedFall_Start, EffectProbedFall_Update },
    { EffectSurroundPlayer_Start, EffectSurroundPlayer_Update },
    { EffectDistanceFade_Start, EffectDistanceFade_Update },
    { EffectTrailHoming_Start, EffectTrailHoming_Update },
    { EffectAimedHoming_Start, EffectAimedHoming_Update },
    { EffectShot_Start, EffectShot_Update },
    { EffectSeekPlayer_Start, EffectSeekPlayer_Update },
    { EffectRay_Start, EffectRay_Update },
    { EffectChain_Start, EffectChain_Update },
    { EffectAimAtPlayer_Start, EffectAimAtPlayer_Update },
    { EffectOwnerFloor_Start, EffectOwnerFloor_Update },
    { EffectLaunchFromOwner_Start, EffectLaunchFromOwner_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectBowDraw_Start, EffectBowDraw_Update },
    { EffectInheritSpeed_Start, EffectInheritSpeed_Update },
    { EffectHomeOnEntity_Start, EffectHomeOnEntity_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectSpiral_Start, EffectSpiral_Update },
    { EffectScatterAroundPlayer_Start, EffectScatterAroundPlayer_Update },
    { EffectDrop_Start, EffectDrop_Update },
    { EffectPitchScale_Start, EffectPitchScale_Update },
    { EffectRepeater_Start, EffectRepeater_Update },
    { EffectImpactShot_Start, EffectImpactShot_Update },
    { EffectFollowLight_Start, EffectFollowLight_Update },
    { EffectLight_Start, EffectLight_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectSound_Start, EffectIdle_Update },
    { EffectWeaponSound_Start, EffectIdle_Update },
    { EffectCorkscrew_Start, EffectCorkscrew_Update },
    { EffectWander_Start, EffectWander_Update },
    { EffectFollowPoseScaled_Start, EffectFollowPoseScaled_Update },
    { EffectConverge_StartInSpread, EffectConverge_Update },
    { EffectJitterWobble_Start, EffectJitterWobble_Update },
    { EffectJitterOrbit_Start, EffectJitterOrbit_Update },
    { EffectSeekPoint_Start, EffectSeekPoint_Update },
    { EffectLoopUntilFlag_Start, EffectLoopUntilFlag_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
    { EffectIdle_Start, EffectIdle_Update },
};
