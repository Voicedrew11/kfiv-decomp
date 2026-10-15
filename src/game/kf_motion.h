/* Motion: the keyed poses of a model's parts, played a step at a time and blended in (kf_motion.c). */
#ifndef KF_MOTION_H
#define KF_MOTION_H

#include "common.h"
#include "kf_entity.h"
#include "kf_model.h"

/* Puts every part of the model in motion slot 0. */
void Motion_ClearPartSlots(EntityModel *model);

/* Puts parts `first` to `last` of the model in motion slot `slot`. */
void Motion_SetPartSlots(EntityModel *model, s32 first, s32 last, s32 slot);

/* Motion_SeekKeys for the parts in motion slot `slot` only. */
int Motion_SeekSlotKeys(ModelInstance *model, int slot, int table_index, int value);

/* Plays one of the entity's motion slots a step on; returns whether the motion ended. */
s32 Motion_StepSlot(Entity *entity, EntityUnkE0SlotBits *slot_bits, s32 slot, s32 mode);

/* Eases the parts of one motion slot toward the pose at `frame`; returns whether `weight` reached 1. */
s32 Motion_BlendSlotToPose(ModelInstance *model, s32 slot, s32 table, s16 frame, f32 weight);

/* Poses the parts of one motion slot between the first keys of two motions. */
s32 Motion_MixSlotFirstKeys(ModelInstance *model, s32 slot, s32 table_a, s32 table_b, f32 ratio);

/* Starts animation `animation` of the entity at `frame`, blending in over `blend_frames`. */
int Motion_Start(Entity *entity, int animation, int frame, u32 blend_frames, int force);

/* Puts every part on the key of motion `table_index` that frame `value` falls in. */
int Motion_SeekKeys(ModelInstance *model, int table_index, int value);

/* Plays the entity's motion a step on; returns whether the motion ended. */
s32 Motion_Step(Entity *entity, s32 mode);

/* Poses every part at the model's playback position and steps it; returns whether the motion ended. */
s32 Motion_StepModel(ModelInstance *model, s32 step, s32 backwards);

/* Motion_StepModel, blending the pose with the current one by `weight`. */
s32 Motion_StepModelBlended(ModelInstance *model, s32 step, s32 backwards, f32 weight);

/* Eases every part toward the pose at the channel's frame and counts the blend frames down. */
void Motion_BlendToPose(ModelInstance *model, f32 weight);

/* Poses every part between the first keys of two motions. */
s32 Motion_MixFirstKeys(ModelInstance *model, s32 table_a, s32 table_b, f32 ratio);

#endif
