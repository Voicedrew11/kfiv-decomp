#include "kf_collision.h"

/* the collision mesh a model has, read through the model each time (the copy of the pointer that
   retail keeps in a register comes from that) */
#define MODEL_MESH(model) ((CollisionMesh *)(model)->strips)

/* Makes a collision mesh that was just loaded usable: turns its offsets into pointers and, if
   asked, its point numbers into scratchpad addresses. Returns where the mesh ends. */
char *func_00112400(char *data, ModelInstance *model, int use_scratchpad) {
    char *cursor = data;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    int *offsets;
    int i;
    int j;

    model->strips = (ModelStripList *)cursor;
    cursor += 0x50;
    MODEL_MESH(model)->strips = (CollisionStrip *)cursor;
    strip = (CollisionStrip *)cursor;
    /* the six strip lists and the two words after them are offsets from the mesh */
    offsets = (int *)&MODEL_MESH(model)->unk18;
    for (i = 0; i < 8; i++) {
        offsets[i] = (int)data + offsets[i];
    }
    for (i = 0; (u32)i < MODEL_MESH(model)->strip_count; i++) {
        point = strip->points;
        cursor += 0x20;
        if (use_scratchpad) {
            for (j = 0; j < strip->point_count; j++) {
                point->position = &COLLISION_WORLD_POINTS[(int)point->position];
                point++;
            }
        } else {
            point += strip->point_count;
        }
        cursor += strip->point_count * sizeof(CollisionStripPoint);
        strip = (CollisionStrip *)point;
    }
    MODEL_MESH(model)->unk08 = cursor;
    return cursor + MODEL_MESH(model)->unk0C * 0x10;
}
