/* Model motions: loading them into a ModelChannel, playing a frame (Hermite between keys) and blending in. */
#include "model/model.h"

void *memset(void *, int, u32);
MotionData *ModelMotion_Load(MotionData *, ModelInstance *, int);
void ModelMotion_Hermite(void *, void *, void *, void *, void *, f32);
void ModelMotion_StepBlend(ModelInstance *);
float Angle_Wrap(float);
void ModelMotion_Select(ModelInstance *, int);
void Vector_WeightedSum4(void *, void *, void *, void *, void *, float *);

/* Adds a motion to the model's channel and poses the parts on its first key; returns the end of the motion. */
MotionData *ModelMotion_Load(MotionData *data, ModelInstance *model, int unused) {
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
        channel->frame_step = 0x100;
        channel->motion_index = limit;
        channel->limit = limit + 1;
        channel->frame = 0;
        channel->frame_fixed = 0;
        channel->blend_left = 0;
        model->model_flags |= 1;
        channel->tables[channel->motion_index] = motion->parts;
        motion->vectors = (MotionVector *)((char *)motion + motion->vectors_offset);
    } while (0);
    model->part_count = motion->part_count;
    // match: the parameter is reused as a cursor nothing reads (cleared, stepped per part, set to the end below)
    for (data = NULL; i < model->part_count; i++, data++) {
        model->parts[i].level = 0;
        if (channel->tables[channel->motion_index][i].keys_offset != 0 &&
            channel->tables[channel->motion_index][i].thresholds == NULL) {
            channel->tables[channel->motion_index][i].thresholds =
                (u16 *)((char *)motion + channel->tables[channel->motion_index][i].keys_offset);
        }
        parts = model->parts;
        vectors = motion->vectors;
        key = (MotionKey *)channel->tables[channel->motion_index][i].thresholds;
        vector = &vectors[key->position];
        parts[i].position[0] = vector->x;
        parts[i].position[1] = vector->y;
        parts[i].position[2] = vector->z;
        parts[i].position[3] = 1.0f;
        vector = &vectors[key->rotation];
        parts[i].rotation_x = vector->x;
        parts[i].rotation_y = vector->y;
        parts[i].rotation_z = vector->z;
        parts[i].rotation_w = 1.0f;
        vector = &vectors[key->scale];
        parts[i].scale[0] = vector->x;
        parts[i].scale[1] = vector->y;
        parts[i].scale[2] = vector->z;
        if (channel->tables[channel->motion_index][i].child != 0) {
            parts[i].child = &parts[channel->tables[channel->motion_index][i].child];
        } else {
            parts[i].child = NULL;
        }
        if (channel->tables[channel->motion_index][i].sibling != 0) {
            parts[i].sibling = &parts[channel->tables[channel->motion_index][i].sibling];
        } else {
            parts[i].sibling = NULL;
        }
        // match: indexing level_tables as an array swaps the addu operands
        parts[i].part_flags = (*(ModelPartLevels **)((char *)model->level_tables + (channel->motion_index << 2)))[i].unk02;
    }
    data = (MotionData *)((char *)motion + motion->size);
    return data;
}

/* Gives the model a fresh ModelChannel and loads every motion of the file; returns the end of the file.
   `mode` is handed on to ModelMotion_Load, which ignores it. */
MotionData *ModelMotion_LoadFile(MotionFile *file, ModelChannel **alloc, ModelInstance *model, int mode) {
    MotionData *motion = (MotionData *)file;   // match: the cursor is its own variable; the file stays in $4
    u32 i;

    model->motion_file = file;
    motion = (MotionData *)((MotionFile *)motion)->entries;
    motion = (MotionData *)((MotionFileEntry *)motion + file->motion_count);
    MODEL_CHANNEL(model) = *alloc;
    memset(MODEL_CHANNEL(model), 0, sizeof(ModelChannel));
    (*alloc)++;
    for (i = 0; i < model->motion_file->motion_count; i++) {
        motion = ModelMotion_Load(motion, model, mode);
    }
    return motion;
}

/* Switches the model to one of its loaded motions, at frame 0. */
void ModelMotion_Select(ModelInstance *model, int index) {
    int i;

    MODEL_CHANNEL(model)->motion_index = index;
    MODEL_CHANNEL(model)->frame = 0;
    MODEL_CHANNEL(model)->frame_fixed = 0;
    for (i = 0; i < model->part_count; i++) {
        model->parts[i].level = 0;
    }
}

/* Whether the model's motion has reached its last frame. */
int ModelMotion_IsAtEnd(ModelInstance *model) {
    ModelChannel *channel = MODEL_CHANNEL(model);

    return channel->frame >= ((ModelTableHeader *)channel->tables[channel->motion_index] - 1)->unk00;
}

/* Whether the model's motion stepped onto the given frame on its last step. */
int ModelMotion_JustReachedFrame(ModelInstance *model, int frame) {
    ModelChannel *channel = MODEL_CHANNEL(model);

    return channel->frame_changed != 0 && channel->frame == frame;
}

/* Plays one frame of the model's current motion: every part is posed between the two keys around the frame. */
void ModelMotion_Step(void *model_arg) {
    ModelInstance *model = model_arg;
    Vector4 *point0 = &MOTION_SCRATCH_POINTS[0];
    Vector4 *point1 = &MOTION_SCRATCH_POINTS[1];
    Vector4 *tangent0 = &MOTION_SCRATCH_POINTS[2];
    Vector4 *tangent1 = &MOTION_SCRATCH_POINTS[3];
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
        if (channel->blend_left != 0) {
            ModelMotion_StepBlend(model);
            return;
        }
        // match: two steps (the MotionData header is as long as one ModelPartLevels)
        motion = (MotionData *)channel->tables[channel->motion_index];
        motion = (MotionData *)((ModelPartLevels *)motion - 1);
        record = channel->tables[channel->motion_index];
        vectors = motion->vectors;
        for (i = 0; i < model->part_count; i++, record++) {
            part = &model->parts[i];
            if (record->thresholds != NULL && record->count != 0) {
                // match: the level is read twice, as a shift and mask of the whole doubleword
                key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
                level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
                while (level < record->count && channel->frame >= key->frame) {
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
                    elapsed = channel->frame_fixed - (elapsed << 8);
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
                    ModelMotion_Hermite(&part->rotation_x, point0, point1, tangent0, tangent1, ratio);
                    part->rotation_x = Angle_Wrap(part->rotation_x);
                    part->rotation_y = Angle_Wrap(part->rotation_y);
                    part->rotation_z = Angle_Wrap(part->rotation_z);

                    *(MotionVector *)point0 = vectors[key->position];
                    *(MotionVector *)point1 = vectors[next->position];
                    *(MotionVector *)tangent0 = vectors[key->position_out];
                    *(MotionVector *)tangent1 = vectors[next->position_in];
                    ModelMotion_Hermite(part->position, point0, point1, tangent0, tangent1, ratio);

                    part->scale[0] = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
                    part->scale[1] = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
                    part->scale[2] = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
                }
            }
        }
        old_frame = channel->frame;
        channel->frame_fixed += channel->frame_step;
        channel->frame = channel->frame_fixed >> 8;
        channel->frame_changed = old_frame != channel->frame;
        if (motion->frame_count < channel->frame) {
            channel->frame = 0;
            channel->frame_fixed = 0;
            for (i = 0; i < model->part_count; i++) {
                model->parts[i].level = 0;
            }
        }
    }
}

/* Switches the model to a motion and blends its pose into the motion's first key over `frames` frames;
   returns 0. */
int ModelMotion_BlendInto(ModelInstance *model, u8 index, u16 speed) {
    ModelMotion_Select(model, index);
    MODEL_CHANNEL(model)->blend_left = speed << 8;
    return 0;
}

/* The last frame of the model's current motion. */
u16 ModelMotion_GetLastFrame(char *model_arg) {
    ModelInstance *model = (ModelInstance *)model_arg;
    ModelChannel *channel = MODEL_CHANNEL(model);

    return ((ModelTableHeader *)channel->tables[channel->motion_index] - 1)->unk00;
}

/* Hermite interpolation: hands the four basis weights for the ratio to Vector_WeightedSum4. */
void ModelMotion_Hermite(void *result, void *point0, void *point1, void *tangent0, void *tangent1, f32 ratio) {
    f32 weights[4];
    f32 squared = ratio * ratio;
    f32 cubed = squared * ratio;

    weights[0] = cubed + cubed - 3.0f * squared + 1.0f;
    weights[1] = -2.0f * cubed + 3.0f * squared;
    weights[2] = cubed - (squared + squared) + ratio;
    weights[3] = cubed - squared;
    Vector_WeightedSum4(result, point0, point1, tangent0, tangent1, weights);
}

/* One frame of blending the pose into the first key of the current motion (while ModelChannel.value is not 0):
   every part moves 1/divisor of the way, and value drops by word until it reaches 0. */
void ModelMotion_StepBlend(ModelInstance *model) {
    ModelChannel *channel = MODEL_CHANNEL(model);
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    f32 divisor;
    f32 unused[4]; // match: the frame has 16 bytes nothing uses
    int i;

    // match: two steps, as in ModelMotion_Step (the MotionData header is as long as one ModelPartLevels)
    motion = (MotionData *)channel->tables[channel->motion_index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = channel->tables[channel->motion_index];
    vectors = motion->vectors;
    if (channel->frame_step >= channel->blend_left) {
        divisor = 1.0f;
    } else if (channel->unk8F == 0) {
        divisor = (f32)channel->blend_left / (f32)channel->frame_step;
    } else {
        divisor = 255.0f / (f32)channel->unk8F;
    }
    part = model->parts;
    for (i = 0; i < model->part_count; i++, record++, part++) {
        key = (MotionKey *)record->thresholds;
        if (key != NULL) {
            part->rotation_x += (vectors[key->rotation].x - part->rotation_x) / divisor;
            part->rotation_y += (vectors[key->rotation].y - part->rotation_y) / divisor;
            part->rotation_z += (vectors[key->rotation].z - part->rotation_z) / divisor;
            part->position[0] += (vectors[key->position].x - part->position[0]) / divisor;
            part->position[1] += (vectors[key->position].y - part->position[1]) / divisor;
            part->position[2] += (vectors[key->position].z - part->position[2]) / divisor;
            part->scale[0] += (vectors[key->scale].x - part->scale[0]) / divisor;
            part->scale[1] += (vectors[key->scale].y - part->scale[1]) / divisor;
            part->scale[2] += (vectors[key->scale].z - part->scale[2]) / divisor;
        }
    }
    if (channel->frame_step >= channel->blend_left) {
        channel->blend_left = 0;
    } else {
        channel->blend_left -= channel->frame_step;
    }
}
