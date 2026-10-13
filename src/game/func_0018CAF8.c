#include "kf_entity.h"
#include "kf_map.h"
#include "kf_player.h"

extern int D_00446770;
extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;

void func_00216778(void *, void *);
void Vector_Subtract(void *, void *, void *);
void func_00216D20(void *, void *);
s32 func_0018AB40(void *, f32 *, f32 *, f32, s32);

/* whether something stands between the entity and the player: tests the line from the entity's
   middle to the player's against the entity's own map record and the player's */
s32 func_0018CAF8(Entity *entity) {
    f32 position[4];
    f32 target[4];
    f32 distance_out[4];
    MapRecord *player_record;
    f32 distance;
    s32 hit;

    // match: index first, or the two address registers swap
    player_record = &((MapRecord *)(D_00446770 + D_0038CD20)->records)[D_00413F80.map_index];
    func_00216778(position, entity->model->position);
    position[1] -= entity->unk34 * 0.5f;
    func_00216778(target, &D_00413F80);
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
    Vector_Subtract(target, target, position);
    func_00216D20(target, target);
    if (entity->unk14 != player_record && (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) != 0x4000000) {
        hit = func_0018AB40(entity->unk14, position, target, distance, 0);
        if (hit != 0) {
            return hit;
        }
    }
    return func_0018AB40(player_record, position, target, distance, 0);
}
