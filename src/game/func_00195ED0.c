#include "kf_entity.h"

int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);

int func_00195ED0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 elapsed, s32 unk82) {
    return EntityShot_LaunchAimed(entity, unk3D8, elapsed, unk82, NULL, 0.0f, 0.0f);
}
