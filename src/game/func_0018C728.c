#include "kf_entity.h"
#include "kf_player.h"
#include "kf_collision.h"

extern PlayerState D_00413F80;
extern int D_00446770;
extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[];
extern MapObject D_003AFA30[];

void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Normalize(void *, void *);
void func_001C2128(void *, int, int, float);

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        if (D_00446770 != map) {
            return D_003AFA30;
        }
        return D_003A1A30;
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        return D_004467B8[D_00446770 == map ? 0 : 1];
    }
    return 0;
}

/* whether a map object stands between the entity's middle and the player's */
s32 func_0018C728(Entity *entity) {
    f32 target[4];
    f32 position[4];
    f32 direction[4];
    f32 lowest[4];
    f32 highest[4];
    f32 distance_out[4];
    MapObject *object;
    f32 distance;
    f32 reach;
    s32 map;
    s32 count;
    s32 i;

    Vector_Copy(position, entity->model->position);
    position[1] -= entity->unk34 * 0.5f;
    Vector_Copy(target, &D_00413F80);
    target[1] -= 115.0f;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(target), "r"(position), "r"(distance_out) : "memory");
    distance = distance_out[0];
    Vector_Subtract(direction, target, position);
    Vector_Normalize(direction, direction);
    if (target[0] < position[0]) {
        lowest[0] = target[0];
        highest[0] = position[0];
    } else {
        lowest[0] = position[0];
        highest[0] = target[0];
    }
    if (target[1] < position[1]) {
        lowest[1] = target[1];
        highest[1] = position[1];
    } else {
        lowest[1] = position[1];
        highest[1] = target[1];
    }
    if (target[2] < position[2]) {
        lowest[2] = target[2];
        highest[2] = position[2];
    } else {
        lowest[2] = position[2];
        highest[2] = target[2];
    }
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (object->unk04->kind == 1 || object->unk04->kind == 2 || object->unk04->kind == 0x11 || object->unk04->kind == 6) {
                reach = object->unk14;
                if (reach <= 0.0f) {
                    continue;
                }
                if (object->model->position.y < lowest[1]) {
                    continue;
                }
                if (object->model->position.x + reach < lowest[0]) {
                    continue;
                }
                if (highest[0] < object->model->position.x - reach) {
                    continue;
                }
                if (object->model->position.z + reach < lowest[2]) {
                    continue;
                }
                if (highest[2] < object->model->position.z - reach) {
                    continue;
                }
                func_001C2128(object, (int)position, (int)direction, distance);
                if (COLLISION_RESULT->hit != 0) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
