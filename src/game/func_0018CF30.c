#include "kf_entity.h"

float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
void *func_001B1390();
char *func_0010CAB8(char *, u32);
void func_00216778(void *, void *);
void func_001B4F30(u8, void *, void *, void *, void *);

/* func_00194FB8, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* func_00194F80 inlined: the distance between two points, height left out */
static inline f32 flat_distance(f32 *from, f32 *target) {
    f32 distance_out[4];

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(from), "r"(target), "r"(distance_out) : "memory");
    return distance_out[0];
}

/* func_00195018 inlined: `current` moved one step of `rate` towards `goal` */
static inline f32 approach_angle(f32 current, f32 goal, f32 rate) {
    f32 difference;
    f32 wrapped;
    s32 steps;

    if (current == goal) {
        return goal;
    }
    if (rate == 0.0f) {
        return current;
    }
    difference = goal - current;
    wrapped = difference - (f32)(s32)(difference / 6.2831855f) * 6.2831855f;
    if (wrapped > 3.1415927f) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    steps = (s32)(wrapped / rate);
    if (steps > 0) {
        current += rate;
    } else if (steps < 0) {
        current -= rate;
    } else {
        current = goal;
    }
    return wrap_angle(current);
}

/* launches the shots of a record whose frame lies in (frame_from, frame_to], each along its
   bone, turned towards `target` by at most the two rates */
int func_0018CF30(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->unk10[i].unk02 != 0 && frame_from < record->unk10[i].unk02 && record->unk10[i].unk02 <= frame_to) {
            shot = func_001B1390();
            shot->unk22 = shot->unk20 = 0;
            shot->unk00 = 1;
            shot->unk28 = entity->params->unk238;
            from = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
            tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01 + 1);
            if (from != NULL && tip != NULL) {
                if (record->unk10[i].unk01 != 0xFF) {
                    delta_x = tip[0] - from[0];
                    delta_y = tip[1] - from[1];
                    delta_z = tip[2] - from[2];
                    angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles[1] = wrap_angle(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles[2] = angles[3] = 0.0f;
                } else {
                    func_00216778(angles, entity->model->unkD0);
                    angles[1] = wrap_angle(angles[1] + 3.1415927f);
                }
                if (target != NULL) {
                    pitch = wrap_angle(-atan2(target[1] - from[1], flat_distance(from, target)));
                    yaw = atan2f(target[0] - from[0], target[2] - from[2]);
                    angles[0] = approach_angle(angles[0], pitch, pitch_rate);
                    angles[1] = approach_angle(angles[1], yaw, yaw_rate);
                }
                func_001B4F30(record->unk10[i].unk00, shot, from, angles, entity);
            }
        }
    }
    return 0;
}
