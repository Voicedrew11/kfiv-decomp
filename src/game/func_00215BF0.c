#include "kf_script.h"
#include "kf_entity_world.h"
#include "kf_entity.h"

int func_00211D90(void *, double); // match: retail converts the float argument to double at the call site (fptodp)

/* Script command: store the entity's model position in x, y and z. */
void func_00215BF0(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        func_00211D90(&command->x, entity->model->position[0]);
        func_00211D90(&command->y, entity->model->position[1]);
        func_00211D90(&command->z, entity->model->position[2]);
    } else {
        func_00211D90(&command->x, 0);
        func_00211D90(&command->y, 0);
        func_00211D90(&command->z, 0);
    }
    command->active = 0;
}
