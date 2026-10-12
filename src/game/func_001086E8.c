#include "kf_model.h"

/* Adds a motion to the model's channel and poses the parts on its first key; returns the end of the motion. */
MotionData *func_001086E8(MotionData *data, ModelInstance *model, int unused) {
    MotionData *motion = data;
    ModelChannel *channel;
    int i;
    ModelPart *parts;
    MotionKey *key;
    MotionVector *vectors;
    MotionVector *vector;
    u8 limit;

    // match: retail was scheduled with a loop boundary before the channel load and after the vectors fix-up
    do {
        channel = MODEL_CHANNEL(model);
        i = 0;
        limit = channel->limit;
        channel->word = 0x100;
        channel->index = limit;
        channel->limit = limit + 1;
        channel->param = 0;
        channel->param_shifted = 0;
        channel->value = 0;
        model->unk100 |= 1;
        channel->tables[channel->index] = motion->parts;
        motion->vectors = (MotionVector *)((char *)motion + motion->vectors_offset);
    } while (0);
    model->part_count = motion->part_count;
    // match: the parameter is reused as a cursor nothing reads (cleared, stepped per part, set to the end below)
    for (data = NULL; i < model->part_count; i++, data++) {
        model->parts[i].level = 0;
        if (channel->tables[channel->index][i].keys_offset != 0 &&
            channel->tables[channel->index][i].thresholds == NULL) {
            channel->tables[channel->index][i].thresholds =
                (u16 *)((char *)motion + channel->tables[channel->index][i].keys_offset);
        }
        parts = model->parts;
        vectors = motion->vectors;
        key = (MotionKey *)channel->tables[channel->index][i].thresholds;
        vector = &vectors[key->position];
        parts[i].unk40[0] = vector->x;
        parts[i].unk40[1] = vector->y;
        parts[i].unk40[2] = vector->z;
        parts[i].unk40[3] = 1.0f;
        vector = &vectors[key->rotation];
        parts[i].unk50 = vector->x;
        parts[i].unk54 = vector->y;
        parts[i].unk58 = vector->z;
        parts[i].unk5C = 1.0f;
        vector = &vectors[key->scale];
        parts[i].unk60[0] = vector->x;
        parts[i].unk60[1] = vector->y;
        parts[i].unk60[2] = vector->z;
        if (channel->tables[channel->index][i].child != 0) {
            parts[i].child = &parts[channel->tables[channel->index][i].child];
        } else {
            parts[i].child = NULL;
        }
        if (channel->tables[channel->index][i].sibling != 0) {
            parts[i].sibling = &parts[channel->tables[channel->index][i].sibling];
        } else {
            parts[i].sibling = NULL;
        }
        // match: indexing level_tables as an array swaps the addu operands
        parts[i].unk78_32 = (*(ModelPartLevels **)((char *)model->level_tables + (channel->index << 2)))[i].unk02;
    }
    data = (MotionData *)((char *)motion + motion->size);
    return data;
}
