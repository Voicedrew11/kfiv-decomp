#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

float atan2f(float, float);
// match: func_001A0CD0.c has not been given its real arguments yet; a cast at the call would lose the tail call
void func_001A0CD0(float, int, int, char, float, float);

/* tells func_001A0CD0 where the entity is as seen from the player */
void func_0018DA38(Entity *entity, s32 unknown1, s32 unknown2, s32 skip_if_unk2A) {
    f32 angle;

    angle = atan2f(entity->model->position[0] - D_00413F80.position.x, entity->model->position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->unk2A != 0 && skip_if_unk2A != 0) {
        return;
    }
    func_001A0CD0(entity->params->unk42E, (u8)unknown1, (u8)unknown2, entity->unkD2, entity->unk60, angle);
}
