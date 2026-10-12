#include "kf_entity.h"

void *memset(void *, int, u32);
void func_00216778(void *, void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

/* func_00194FB8, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

s32 func_00195BD8(Entity *entity, EntityParamsUnk240 *record, s32 index, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;

    memset(&request, 0, sizeof(request));
    memset(&extra, 0, sizeof(extra));
    func_00216778(request.position, entity->model->position);
    yaw = angle + 3.1415927f;
    request.unk30 = entity->radius + record->unk10[index].unk20;
    request.unk34 = entity->height + 10.0f;
    request.unk38 = wrap_angle(yaw);
    request.unk3C = wrap_angle(record->unk10[index].unk22 * 0.017453292f);
    request.owner = entity;
    request.unk4C_0 = 3;
    request.unk4D = 1;
    request.unk50 = record->unk10[index].unk00;
    request.unk62 = record->unk10[index].unk12;
    request.unk4E = record->unk0C;
    request.unk4C_3 = record->unk10[index].unk1E & 1;
    if ((ENTITY_PART_FLAGS(&record->unk10[index])->flags >> 1) & 1) {
        request.unk71 = record->unk10[index].unk1F;
    }
    extra.unk00 = 0;
    extra.unk02 = 0;
    extra.unk20 = 0;
    extra.unk22 = 0;
    return func_001B1470(&request, &extra);
}
