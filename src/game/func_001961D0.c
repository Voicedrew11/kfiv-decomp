#include "kf_entity.h"

s32 Bone_TurnPart(ModelPart *, f32 *, s32, f32 *, f32 *, f32);

/* the other arguments are passed through to Bone_TurnPart as they came */
s32 func_001961D0(Entity *entity, f32 *angles, s32 mode, f32 *goal, f32 *limits, f32 rate) {
    ModelPart *part = (ModelPart *)entity->model->unk134;
    s32 result;

    if (part == NULL) {
        result = FALSE;
    } else {
        result = Bone_TurnPart(part, angles, mode, goal, limits, rate);
    }
    return result;
}
