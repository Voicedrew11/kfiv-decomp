#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

double sqrt(double);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216D20(void *, void *);
s32 func_0018B150(f32 *, f32 *, f32, f32 *);

/* tests the line from the entity's middle to the player's with func_0018B150 */
s32 func_0018CC60(Entity *entity) {
    f32 position[4];
    f32 target[4];
    f32 bounds[4];
    f32 distance;

    func_00216778(position, entity->model->position);
    position[1] -= entity->unk34 * 0.5f;
    func_00216778(target, &D_00413F80);
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
    KF_Vector_Subtract(target, target, position);
    distance = sqrt(target[0] * target[0] + target[1] * target[1] + target[2] * target[2]);
    func_00216D20(target, target);
    return func_0018B150(position, target, distance, bounds);
}
