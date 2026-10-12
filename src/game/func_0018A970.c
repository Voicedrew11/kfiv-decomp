#include "kf_collision.h"
#include "kf_entity.h"

void KF_Vector_Subtract(void *, void *, void *);
void func_00216D20(void *, void *);
void func_00194F48(f32 *, f32 *);
void func_001C2128(void *, int, int, float);

/* Tests the way from `from` to `destination` against `target`; TRUE when something is hit. */
s32 func_0018A970(Entity *target, f32 *from, PlayerData *destination) {
    f32 direction[4];

    KF_Vector_Subtract(direction, destination, from);
    func_00216D20(direction, direction);
    // match: func_00194F48 returns the distance (other files declare it void)
    func_001C2128(target, (int)from, (int)direction, ((f32 (*)(f32 *, void *))func_00194F48)(from, destination));
    return COLLISION_RESULT->hit;
}
