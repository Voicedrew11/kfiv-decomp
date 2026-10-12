#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
f32 func_00194F48(f32 *, f32 *);
float func_001954A8(float *, float *);
float func_001957B8(float, float);

/* Script command: each frame, steer the entity toward the point (x, y, z) until it is within `distance`. */
void func_00215520(ScriptWalkToCommand *command) {
    f32 point[4];
    Entity *entity;
    f32 heading;
    f32 turn;
    f32 rise;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL) {
        point[0] = command->x.value;
        point[1] = command->y.value;
        point[2] = command->z.value;
        point[3] = 0.0f;
        if (func_00194F48(point, entity->model->position) < command->distance.value) {
            command->active = 0;
            return;
        }
        if ((entity->flags & 0xF) == 3 && entity->unk2C != 0) {
            heading = func_001954A8(entity->model->position, point);
            turn = func_001957B8(entity->model->unkD0[1], heading);
            if (ABS(turn) > entity->params->turn_limit) {
                if (turn >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            rise = point[1] - entity->model->position[1];
            if (ABS(rise) > entity->params->unk54) {
                if (rise >= 0.0f) {
                    entity->unkC8 |= 0x20;
                } else {
                    entity->unkC8 |= 0x10;
                }
            }
            entity->unkC8 |= 1;
            if (command->run.value != 0) {
                entity->unkC8 |= 0x40;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = command->mode.value;
            return;
        }
    }
    command->active = 0;
}
