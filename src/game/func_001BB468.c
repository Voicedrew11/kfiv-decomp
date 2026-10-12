#include "kf_player.h"

void func_00216D20(void *dst, void *src);
extern PlayerState D_00413F80;

/* Sets the player aim vector (+0x220) to the flattened, normalised direction from target, and its speed. */
void func_001BB468(Vector4 *target, f32 speed) {
    D_00413F80.aim.x = D_00413F80.position.x - target->x;
    D_00413F80.aim.y = 0.0f;
    D_00413F80.aim.z = D_00413F80.position.z - target->z;
    func_00216D20(&D_00413F80.aim, &D_00413F80.aim);
    D_00413F80.aim_speed = speed;
}
