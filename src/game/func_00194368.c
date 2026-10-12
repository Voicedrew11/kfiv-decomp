#include "kf_model.h"

void func_0019A590(void *, void *, void *, void *, void *, f32);

/* Eases every part toward the pose between the motion keys around the channel's frame, then counts the channel's value down. */
void func_00194368(ModelInstance *model, f32 weight) {
    Vector4 *point0 = (Vector4 *)0x70000000;
    Vector4 *point1 = (Vector4 *)0x70000010;
    Vector4 *tangent0 = (Vector4 *)0x70000020;
    Vector4 *tangent1 = (Vector4 *)0x70000030;
    Vector4 *result = (Vector4 *)0x70000040;
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
    f32 diff;
    f32 angle;
    f32 angle_y;

    channel = MODEL_CHANNEL(model);
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = channel->tables[channel->index];
    vectors = motion->vectors;
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (record->thresholds != NULL && record->count != 0) {
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && channel->param >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
            } else if (level != 0) {
                part->level = level - 1;
                next = key;
                key = next - 1;
                ratio = (f32)(channel->param - key->frame) / (f32)key->length;

                *(MotionVector *)point0 = vectors[key->rotation];
                point0->w = 1.0f;
                *(MotionVector *)point1 = vectors[next->rotation];
                point1->w = 1.0f;
                *(MotionVector *)tangent0 = vectors[key->rotation_out];
                tangent0->w = 1.0f;
                *(MotionVector *)tangent1 = vectors[next->rotation_in];
                tangent1->w = 1.0f;
                func_0019A590(result, point0, point1, tangent0, tangent1, ratio);
                diff = result->x - part->unk50;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk50 += diff * weight;
                diff = result->y - part->unk54;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk54 += diff * weight;
                diff = result->z - part->unk58;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk58 += diff * weight;
                angle = part->unk50;
                angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                part->unk50 = angle;
                angle_y = part->unk54;
                angle_y -= (f32)(s32)(angle_y / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle_y) {
                    angle_y -= 6.2831855f;
                } else if (angle_y < -3.1415927f) {
                    angle_y += 6.2831855f;
                }
                part->unk54 = angle_y;
                angle = part->unk58;
                angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                part->unk58 = angle;

                *(MotionVector *)point0 = vectors[key->position];
                *(MotionVector *)point1 = vectors[next->position];
                *(MotionVector *)tangent0 = vectors[key->position_out];
                *(MotionVector *)tangent1 = vectors[next->position_in];
                func_0019A590(result, point0, point1, tangent0, tangent1, ratio);
                part->unk40[0] += (result->x - part->unk40[0]) * weight;
                part->unk40[1] += (result->y - part->unk40[1]) * weight;
                part->unk40[2] += (result->z - part->unk40[2]) * weight;

                result->x = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
                result->y = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
                result->z = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
                part->unk60[0] += (result->x - part->unk60[0]) * weight;
                part->unk60[1] += (result->y - part->unk60[1]) * weight;
                part->unk60[2] += (result->z - part->unk60[2]) * weight;
            }
        }
    }
    channel->value--;
}
