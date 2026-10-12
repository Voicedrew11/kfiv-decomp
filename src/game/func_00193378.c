#include "kf_model.h"

void func_0019A590(void *, void *, void *, void *, void *, f32);

/* Poses every part between the motion keys around the model's playback position, steps the position and reports whether it wrapped. */
s32 func_00193378(ModelInstance *model, s32 step, s32 backwards) {
    Vector4 *point0 = (Vector4 *)0x70000000;
    Vector4 *point1 = (Vector4 *)0x70000010;
    Vector4 *tangent0 = (Vector4 *)0x70000020;
    Vector4 *tangent1 = (Vector4 *)0x70000030;
    ModelChannel *channel;
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    f32 ratio;
    f32 angle;
    f32 tilt;
    f32 spin;
    u32 elapsed;
    u32 length;
    u16 old_param;
    s32 target;

    channel = MODEL_CHANNEL(model);
    old_param = channel->param;
    motion = (MotionData *)channel->tables[channel->index];
    record = (ModelPartLevels *)motion;
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    vectors = motion->vectors;
    if (backwards != 0) {
        channel->param_shifted -= step;
        target = channel->param_shifted;
        if (target < 0) {
            channel->param = motion->frame_count;
            channel->param_shifted = motion->frame_count << 8;
        } else {
            channel->param = target >> 8;
        }
    } else {
        target = channel->param_shifted + step;
    }
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (target < 0) {
            part->level = record->count - 1;
        }
        if (record->thresholds != NULL) {
            if (record->count == 0) {
                continue;
            }
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && channel->param >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
                continue;
            }
            if (level == 0) {
                continue;
            }
            part->level = level - 1;
            next = key;
            key = next - 1;
            elapsed = key->frame;
            length = key->length;
            elapsed = channel->param_shifted - (elapsed << 8);
            length <<= 8;
            ratio = (f32)elapsed / (f32)length;

            *(MotionVector *)point0 = vectors[key->rotation];
            point0->w = 1.0f;
            *(MotionVector *)point1 = vectors[next->rotation];
            point1->w = 1.0f;
            *(MotionVector *)tangent0 = vectors[key->rotation_out];
            tangent0->w = 1.0f;
            *(MotionVector *)tangent1 = vectors[next->rotation_in];
            tangent1->w = 1.0f;
            func_0019A590(&part->unk50, point0, point1, tangent0, tangent1, ratio);
            angle = part->unk50;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            part->unk50 = angle;
            tilt = part->unk54;
            tilt -= (f32)(s32)(tilt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < tilt) {
                tilt -= 6.2831855f;
            } else if (tilt < -3.1415927f) {
                tilt += 6.2831855f;
            }
            part->unk54 = tilt;
            spin = part->unk58;
            spin -= (f32)(s32)(spin / 6.2831855f) * 6.2831855f;
            if (3.1415927f < spin) {
                spin -= 6.2831855f;
            } else if (spin < -3.1415927f) {
                spin += 6.2831855f;
            }
            part->unk58 = spin;

            *(MotionVector *)point0 = vectors[key->position];
            *(MotionVector *)point1 = vectors[next->position];
            *(MotionVector *)tangent0 = vectors[key->position_out];
            *(MotionVector *)tangent1 = vectors[next->position_in];
            func_0019A590(part->unk40, point0, point1, tangent0, tangent1, ratio);

            part->unk60[0] = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
            part->unk60[1] = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
            part->unk60[2] = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
        }
        if ((motion->frame_count << 8) < target) {
            part->level = 0;
        }
    }
    if (backwards == 0) {
        channel->param_shifted += step;
        channel->param = channel->param_shifted >> 8;
        if (motion->frame_count < channel->param) {
            channel->param = 0;
            channel->param_shifted = 0;
        }
    }
    channel->unk8E = old_param != channel->param;
    if (backwards != 0) {
        return channel->param == motion->frame_count;
    }
    return channel->param == 0;
}
