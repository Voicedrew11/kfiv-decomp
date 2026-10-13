#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_script.h"

int func_00194368(f32, void *); // match: retail leaves a value in $2

/* Each frame, blend the target entity's model toward its motion by `weight` while it is active. */
int func_00215298(ScriptMotionBlendCommand *command) {
    Entity *entity;
    EntityModel *entity_model;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        entity_model = entity->model;
        if (entity_model->unkFC->unk88 != 0) {
            return func_00194368(command->weight.value, entity_model);
        }
    }
    command->active = 0;
}
