#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern char *D_004467C0;

float atan2f(float, float);
// match: func_001A0CD0.c has not been given its real arguments yet
void func_001A0CD0(float, int, int, char, float, float);

/* starts what animation `animation` has listed for the frames from `frame_from` up to `frame_to`
   (both in 1/256 frames), telling func_001A0CD0 where the entity is as seen from the player */
void func_0018D8B8(Entity *entity, s32 animation, s32 frame_from, s32 frame_to) {
    EntityParamsUnk1B0 *record;
    s32 count;
    s32 i;
    s32 frame;
    f32 angle;

    record = (EntityParamsUnk1B0 *)(D_004467C0 + entity->params->unk1B0[animation]);
    count = entity->params->unk110[animation];
    if (count > 0 && record != NULL) {
        angle = atan2f(entity->model->position[0] - D_00413F80.position.x, entity->model->position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (angle > 3.1415927f) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        for (i = 0; i < count; i++, record++) {
            frame = record->unk02 << 8;
            if (frame < frame_from) {
                continue;
            }
            if (frame >= frame_to) {
                continue;
            }
            if (entity->unk2A != 0 && record->unk04 != 0) {
                continue;
            }
            func_001A0CD0(entity->params->unk42E, record->unk00, record->unk01, entity->unkD2, entity->unk60, angle);
        }
    }
}
