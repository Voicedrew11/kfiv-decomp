#include "kf_model.h"

/* One frame of blending the pose into the first key of the current motion (while ModelChannel.value is not 0):
   every part moves 1/divisor of the way, and value drops by word until it reaches 0. */
void func_00109178(ModelInstance *model) {
    ModelChannel *channel = MODEL_CHANNEL(model);
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    f32 divisor;
    f32 unused[4]; // match: the frame has 16 bytes nothing uses
    int i;

    // match: two steps, as in func_00108AC8 (the MotionData header is as long as one ModelPartLevels)
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = channel->tables[channel->index];
    vectors = motion->vectors;
    if (channel->word >= channel->value) {
        divisor = 1.0f;
    } else if (channel->unk8F == 0) {
        divisor = (f32)channel->value / (f32)channel->word;
    } else {
        divisor = 255.0f / (f32)channel->unk8F;
    }
    part = model->parts;
    for (i = 0; i < model->part_count; i++, record++, part++) {
        key = (MotionKey *)record->thresholds;
        if (key != NULL) {
            part->unk50 += (vectors[key->rotation].x - part->unk50) / divisor;
            part->unk54 += (vectors[key->rotation].y - part->unk54) / divisor;
            part->unk58 += (vectors[key->rotation].z - part->unk58) / divisor;
            part->unk40[0] += (vectors[key->position].x - part->unk40[0]) / divisor;
            part->unk40[1] += (vectors[key->position].y - part->unk40[1]) / divisor;
            part->unk40[2] += (vectors[key->position].z - part->unk40[2]) / divisor;
            part->unk60[0] += (vectors[key->scale].x - part->unk60[0]) / divisor;
            part->unk60[1] += (vectors[key->scale].y - part->unk60[1]) / divisor;
            part->unk60[2] += (vectors[key->scale].z - part->unk60[2]) / divisor;
        }
    }
    if (channel->word >= channel->value) {
        channel->value = 0;
    } else {
        channel->value -= channel->word;
    }
}
