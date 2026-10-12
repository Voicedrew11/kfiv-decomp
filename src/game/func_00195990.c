#include "kf_entity.h"

s32 func_0018CDC0(void *target, f32 *point, f32 max_angle, f32 max_height, f32 max_distance, f32 heading);

/* Runs the check of one launch part, unless the animation frame window [frame_min, frame_max] lies outside the part's frames. */
s32 func_00195990(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame_min, s32 frame_max) {
    EntityModel *model;
    f32 turn;
    f32 wrapped;

    model = entity->model;
    turn = record->unk0E * 0.017453292f;
    if (!(0.0f <= model->unkE0[0])) {
        turn = -turn;
    }
    wrapped = model->unkD0[1] + turn + 3.1415927f;
    wrapped = wrapped - (f32)(s32)(wrapped / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    if ((frame_min < record->unk10[index].unk24 && frame_max < record->unk10[index].unk24) || (record->unk10[index].unk26 < frame_min && record->unk10[index].unk26 < frame_max)) {
        return 0;
    }
    return func_0018CDC0(&record->unk10[index].unk12, model->position, record->unk10[index].unk22 * 0.017453292f, entity->unk34, record->unk10[index].unk20, wrapped);
}
