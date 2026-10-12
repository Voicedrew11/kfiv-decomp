#include "kf_model.h"

/* the largest float: a vertex's distance before it has been measured.
   match: written as a literal, the compiler keeps it in .sdata (D_0044675C) */
#define NOT_MEASURED 3.4028235e38f

float sqrtf(float);

/* Step the colour of every vertex within `radius` of `centre` towards `targets`, one component at
   a time by `steps`. Returns how many components have not arrived yet. */
int func_0010EA08(int model_arg, void *centre_arg, int *steps, int *targets, float radius) {
    ModelInstance *model = (ModelInstance *)model_arg;
    f32 *centre = centre_arg;
    // match: the order of these declarations decides the stack slots of the four loop cursors (gcse hashes
    // `variable + step` by the variable's register number, and the slots follow that order)
    s32 changed;
    s32 i;
    u16 part_count;
    ModelPart *part;
    ModelPartVertex *vertex;
    s32 j;
    s32 count;
    s32 *cursor;
    ModelPartData *block;
    u16 length;
    s32 k;
    s32 vertex_count;
    f32 distance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 component;

    changed = 0;
    i = 0;
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        j = 0;
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        cursor = *(s32 **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            block = (ModelPartData *)cursor;
            length = block->length;
            k = 0;
            cursor = &block->unk70;
            vertex_count = *cursor & 0x7FFF;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                distance = NOT_MEASURED;
                for (component = 0; component < 4; component++) {
                    if (steps[component] > 0 && vertex->colour[component] < targets[component]) {
                        if (distance == NOT_MEASURED) {
                            delta_x = centre[0] - vertex->x;
                            delta_y = centre[1] - vertex->y;
                            delta_z = centre[2] - vertex->z;
                            distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
                        }
                        if (distance <= radius) {
                            if (vertex->colour[component] + steps[component] < targets[component]) {
                                vertex->colour[component] += steps[component];
                            } else {
                                vertex->colour[component] = targets[component];
                            }
                        }
                        changed++;
                    } else if (steps[component] < 0 && targets[component] < vertex->colour[component]) {
                        if (distance == NOT_MEASURED) {
                            delta_x = centre[0] - vertex->x;
                            delta_y = centre[1] - vertex->y;
                            delta_z = centre[2] - vertex->z;
                            distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
                        }
                        if (distance <= radius) {
                            if (targets[component] < vertex->colour[component] + steps[component]) {
                                vertex->colour[component] += steps[component];
                            } else {
                                vertex->colour[component] = targets[component];
                            }
                        }
                        changed++;
                    }
                }
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
    return changed;
}
