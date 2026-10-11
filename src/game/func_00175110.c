#include "kf_entity.h"

extern float D_00413F80[];

double atan(double);
void *memset(void *, int, u32);
float func_00195478(float *, char *);
float func_001957B8(float, float);
float func_00194FB8(float);
void func_001B2C48(void *);
void func_00216778(void *, void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

s32 func_00175110(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;
    f32 pitch;
    f32 spread;

    if (record->unk10[index].unk24 != 0 && !(frame < record->unk10[index].unk24 && current_frame < record->unk10[index].unk24) &&
        !(record->unk10[index].unk26 < frame && record->unk10[index].unk26 < current_frame)) {
        yaw = func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
        pitch = (f32)atan((entity->model->position[1] - D_00413F80[1] + 220.0f) / entity->distance_to_player) + D_00413F80[4];
        if (ABS(yaw) < entity->params->unk432 * 0.017453292f && ABS(pitch) < (s16)entity->params->unk434 * 0.017453292f) {
            spread = func_00194FB8(record->unk10[0].unk22 * 0.017453292f);
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < spread) {
                func_001B2C48(&entity->params->unk240[0].unk10[0].unk12);
                memset(&request, 0, sizeof(request));
                memset(&extra, 0, sizeof(extra));
                func_00216778(request.position, entity->model->position);
                request.unk34 = entity->height + 10.0f;
                request.unk30 = entity->unk38 + record->unk10[index].unk20;
                request.unk38 = func_00194FB8(angle + 3.1415927f);
                request.unk3C = func_00194FB8(record->unk10[index].unk22 * 0.017453292f);
                request.owner = entity;
                request.unk4C_0 = 3;
                request.unk4D = 1;
                request.unk50 = record->unk10[index].unk00;
                request.unk62 = record->unk10[index].unk12;
                request.unk4E = record->unk0C;
                extra.unk00 = 0;
                extra.unk02 = 0;
                extra.unk20 = 0;
                extra.unk22 = 0;
                request.unk4C_3 = record->unk10[index].unk1E & 1;
                return func_001B1470(&request, &extra);
            }
        }
    }
    return 0;
}
