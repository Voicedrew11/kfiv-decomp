#include "kf_model.h"

void func_00109100(void *, void *, void *, void *, void *, f32);
void func_00109178(ModelInstance *);
float Angle_Wrap(float);

/* Plays one frame of the model's current motion: every part is posed between the two keys around the frame. */
void func_00108AC8(void *model_arg) {
    ModelInstance *model = model_arg;
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
    int level;
    int i;
    f32 ratio;
    u32 elapsed;
    u32 length;
    u16 old_frame;

    if (MODEL_UNK100_BITS(model)->has_motion) {
        channel = MODEL_CHANNEL(model);
        if (channel->value != 0) {
            func_00109178(model);
            return;
        }
        // match: two steps (the MotionData header is as long as one ModelPartLevels)
        motion = (MotionData *)channel->tables[channel->index];
        motion = (MotionData *)((ModelPartLevels *)motion - 1);
        record = channel->tables[channel->index];
        vectors = motion->vectors;
        for (i = 0; i < model->part_count; i++, record++) {
            part = &model->parts[i];
            if (record->thresholds != NULL && record->count != 0) {
                // match: the level is read twice, as a shift and mask of the whole doubleword
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
                    // match: both values are read first and scaled afterwards
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
                    func_00109100(&part->unk50, point0, point1, tangent0, tangent1, ratio);
                    part->unk50 = Angle_Wrap(part->unk50);
                    part->unk54 = Angle_Wrap(part->unk54);
                    part->unk58 = Angle_Wrap(part->unk58);

                    *(MotionVector *)point0 = vectors[key->position];
                    *(MotionVector *)point1 = vectors[next->position];
                    *(MotionVector *)tangent0 = vectors[key->position_out];
                    *(MotionVector *)tangent1 = vectors[next->position_in];
                    func_00109100(part->unk40, point0, point1, tangent0, tangent1, ratio);

                    part->unk60[0] = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
                    part->unk60[1] = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
                    part->unk60[2] = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
                }
            }
        }
        old_frame = channel->param;
        channel->param_shifted += channel->word;
        channel->param = channel->param_shifted >> 8;
        channel->unk8E = old_frame != channel->param;
        if (motion->frame_count < channel->param) {
            channel->param = 0;
            channel->param_shifted = 0;
            for (i = 0; i < model->part_count; i++) {
                model->parts[i].level = 0;
            }
        }
    }
}
