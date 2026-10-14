#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

double sqrt(double);
void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Normalize(void *, void *);
s32 func_0018B150(f32 *, f32 *, f32, f32 *);

/* tests the line from the entity's middle to the player's with func_0018B150 */
s32 func_0018CC60(Entity *entity) {
    f32 position[4];
    f32 target[4];
    f32 bounds[4];
    f32 distance;

    Vector_Copy(position, entity->model->position);
    position[1] -= entity->unk34 * 0.5f;
    Vector_Copy(target, &D_00413F80);
    target[1] -= 115.0f;
    if (position[0] < target[0]) {
        bounds[0] = position[0];
        bounds[1] = target[0];
    } else {
        bounds[0] = target[0];
        bounds[1] = position[0];
    }
    if (position[2] < target[2]) {
        bounds[2] = position[2];
        bounds[3] = target[2];
    } else {
        bounds[2] = target[2];
        bounds[3] = position[2];
    }
    Vector_Subtract(target, target, position);
    distance = sqrt(target[0] * target[0] + target[1] * target[1] + target[2] * target[2]);
    Vector_Normalize(target, target);
    return func_0018B150(position, target, distance, bounds);
}
