#include "kf_script.h"
#include "kf_entity.h"

extern PlayerData D_00413F80;

Entity *func_001267B0(int);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
void func_00195898(char *, u8);

/* Script command: put the entity in state 4 (unless it already is in state 5, facing the player when it
   has to), then wait `frames` and for state 5. */
void func_00215310(ScriptStateWaitCommand *command) {
    Entity *entity;
    f32 heading;
    f32 turn;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        if (entity->unkC6 != 0) {
            return;
        }
        if (entity->params->flags & 0x80) {
            switch (command->step) {
            case 0:
                heading = func_001954A8(entity->model->position, D_00413F80.position);
                turn = func_001957B8(entity->model->unkD0[1], heading);
                if (entity->state == 5) {
                    if (ABS(turn) <= entity->params->unk438 * 0.017453292f) {
                        command->active = 0;
                        return;
                    }
                }
                func_00195898((char *)entity, 4);
                command->step++;
                return;
            case 1:
                if (--command->frames.value > 0) {
                    return;
                }
                if (entity->state != 5) {
                    return;
                }
                command->active = 0;
                return;
            default:
                return;
            }
        } else {
            switch (command->step) {
            case 0:
                if (entity->state == 5) {
                    command->active = 0;
                    return;
                }
                func_00195898((char *)entity, 4);
                command->step++;
                return;
            case 1:
                if (--command->frames.value > 0) {
                    return;
                }
                if (entity->state != 5) {
                    return;
                }
                command->active = 0;
                return;
            default:
                return;
            }
        }
    }
    command->active = 0;
}
