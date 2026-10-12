#include "kf_entity.h"

void func_00195588(Vector4 *, float);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
f32 func_00194F48(f32 *, f32 *);
void func_00216D20(void *, void *);
int func_0018AE98(void *, void *, void *, float); // match: the distance is the last argument (set up after the pointers)

/* follows the arc of a jump from the entity's feet and asks func_0018AE98 about the straight
   line to where it ends */
s32 func_00166BF8(Entity *entity) {
    f32 direction[4];
    f32 from[4];
    f32 target[4];
    f32 rise;
    f32 advance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 distance;

    func_00195588((Vector4 *)direction, entity->model->unkD0[1]);
    rise = entity->params->unk43A;
    advance = entity->params->unk43C;
    delta_x = delta_y = delta_z = 0.0f;
    while (rise > 0.0f) {
        delta_x += advance * direction[0];
        delta_z += advance * direction[2];
        delta_y -= rise;
        rise -= entity->params->unk43E * 0.1f;
        advance -= 0.5f;
        if (advance < 0.0f) {
            advance = 0.0f;
        }
    }
    direction[0] = delta_x;
    direction[1] = delta_y - 100.0f;
    direction[2] = delta_z;
    func_00216778(from, entity->model->position);
    from[1] -= entity->unk34;
    func_00216C48(target, from, direction);
    distance = func_00194F48(from, target);
    func_00216D20(direction, direction);
    return func_0018AE98(from, direction, NULL, distance) == 0;
}
