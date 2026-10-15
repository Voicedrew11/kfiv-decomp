#include "common.h"
#include "kf_entity.h"

extern s32 D_00413F80[];

void Vector_Copy(void *, void *);
int Bone_TurnPartToLook(void *, int, int, int, void *, int, float);

/* Forwards to Bone_TurnPartToLook with the player's position (220 lower) when the entity model's unk138 is set
   and its params flag 0x2000 is on. */
int func_00196120(Entity *entity, void *target, s32 mode, void *out, f32 range) {
    f32 position[4];
    int result;

    Vector_Copy(position, D_00413F80);
    position[1] -= 220.0f;
    if (entity->model->unk138 == NULL || !(entity->params->flags & 0x2000)) {
        result = 0;
    } else {
        result = Bone_TurnPartToLook(entity->model, (int)entity->model->unk138, (int)target, mode, position, (int)out, range);
    }
    return result;
}
