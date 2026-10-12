#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
float func_0019B838(float);
int func_00211CF8(ScriptArg *, int);

/* Script command: store 1 in the result when the entity's turn away from unk68 is within half of `range` degrees. */
void func_00215AF8(ScriptFacingCommand *command) {
    Entity *entity;
    f32 angle;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL) {
        angle = func_0019B838(entity->unk68 - entity->model->unkD0[1]);
        angle = ABS(angle);
        angle *= 57.29578f;
        if (angle <= command->range.value / 2) {
            func_00211CF8(&command->result, TRUE);
            command->active = 0;
            return;
        }
    }
    func_00211CF8(&command->result, FALSE);
    command->active = 0;
}
