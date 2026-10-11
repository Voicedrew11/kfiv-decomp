#include "kf_entity.h"
#include "kf_map.h"

void func_001958B8(void *, void (*)(void), int);
extern MapSlot D_0038CD20[];
extern int D_00446770;

/* Copy the actor position to its model, relative to the map origin when on another map. */
void func_0017A158(Entity *entity, int invoke_now) {
    EntityUnkE0 *sub;
    ModelInstance *model;
    MapSlot *slot;
    Vector4 *origin;
    float x;
    float y;
    float z;

    sub = &entity->unkE0;
    model = ENTITY_MODEL(entity);
    x = sub->unk48;
    model->position.x = x;
    y = sub->unk4C;
    model->position.y = y;
    z = sub->unk50;
    model->position.z = z;
    if (entity->group_id != D_00446770) {
        slot = &D_0038CD20[D_00446770];
        origin = &slot->origins[ENTITY_CONTROLLER(entity)->unk43A];
        model->position.x = x - origin->x;
        model->position.y = y - origin->y;
        model->position.z = z - origin->z;
    }
    model->rotation.y = 0.0f;
    func_001958B8(entity, sub->unk18, invoke_now);
}
