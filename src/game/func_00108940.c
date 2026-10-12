#include "kf_model.h"

void *memset(void *, int, u32);
MotionData *func_001086E8(MotionData *, ModelInstance *, int);

/* Gives the model a fresh ModelChannel and loads every motion of the file; returns the end of the file.
   `mode` is handed on to func_001086E8, which ignores it. */
MotionData *func_00108940(MotionFile *file, ModelChannel **alloc, ModelInstance *model, int mode) {
    MotionData *motion = (MotionData *)file;   // match: the cursor is its own variable; the file stays in $4
    u32 i;

    model->motion_file = file;
    motion = (MotionData *)((MotionFile *)motion)->entries;
    motion = (MotionData *)((MotionFileEntry *)motion + file->motion_count);
    MODEL_CHANNEL(model) = *alloc;
    memset(MODEL_CHANNEL(model), 0, sizeof(ModelChannel));
    (*alloc)++;
    for (i = 0; i < model->motion_file->motion_count; i++) {
        motion = func_001086E8(motion, model, mode);
    }
    return motion;
}
