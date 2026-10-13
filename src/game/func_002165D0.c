#include "kf_script.h"
#include "kf_entity_world.h"
#include "kf_entity.h"

float func_0019B838(float);

/* Script command: copy x, y and z into the entity's model unkD0, each passed through func_0019B838. */
void func_002165D0(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        entity->model->unkD0[0] = command->x.value;
        entity->model->unkD0[1] = command->y.value;
        entity->model->unkD0[2] = command->z.value;
        entity->model->unkD0[0] = func_0019B838(entity->model->unkD0[0]);
        entity->model->unkD0[1] = func_0019B838(entity->model->unkD0[1]);
        entity->model->unkD0[2] = func_0019B838(entity->model->unkD0[2]);
    }
    command->active = 0;
}
