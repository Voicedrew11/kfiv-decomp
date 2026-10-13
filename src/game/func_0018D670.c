#include "kf_entity.h"
#include "kf_effect.h"

extern char *D_004467C0;

float atan2f(float, float);
float sqrtf(float);
char *func_0010CAB8(char *, u32);

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

/* starts what animation `animation` has listed for the frames from `frame_from` up to `frame_to`
   (both in 1/256 frames): each at a bone of the model, pointing along it */
void func_0018D670(Entity *entity, s32 animation, s32 frame_from, s32 frame_to) {
    EntityParamsUnk130 *record;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 frame;
    s32 i;

    record = (EntityParamsUnk130 *)(D_004467C0 + entity->params->unk130[animation]);
    count = entity->params->unkF0[animation];
    if (count <= 0) {
        return;
    }
    if (record == NULL) {
        return;
    }
    if (entity->unk14 == NULL) {
        return;
    }
    if (!(((MapRecord *)entity->unk14)->unk38 & 0x4000000)) {
        return;
    }
    for (i = 0; i < count; i++, record++) {
        from = (f32 *)func_0010CAB8((char *)entity->model, record->unk04);
        tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk04 + 1);
        if (from != NULL && tip != NULL) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + 1.5707964f);
            angles[2] = angles[3] = 0.0f;
            frame = record->unk02 << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (record->unk05 != 0) {
                KF_Effect_StartSpawner(entity->params->unk440[(s32)(entity->flags >> 60) & 7], from, angles, entity);
            } else {
                KF_Effect_StartSpawner(record->unk00, from, angles, entity);
            }
        }
    }
}
