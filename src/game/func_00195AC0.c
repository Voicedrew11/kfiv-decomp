#include "kf_entity.h"

int func_00195B68(void *, void *, int, int, int, float);

/* Turns the heading by a signed angle and wraps the result to [-pi, pi]. */
int func_00195AC0(Entity *entity, TurnCommand *command, int extra2, int extra3, int extra4) {
    ModelInstance *model = ENTITY_MODEL(entity);
    float delta = (float)command->degrees * 0.017453292f;
    float angle;

    if (!(model->unkE0 >= 0.0f)) {
        delta = -delta;
    }
    angle = model->rotation.y + delta;
    angle = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return func_00195B68(entity, command, extra2, extra3, extra4, angle);
}
