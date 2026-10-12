#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"

s32 func_001C8F18(s32, s32, s32, f32 *, f32, f32 *, f32 *, f32 *, u32);
int func_001C0E40(f32, void *);
float func_0019B838(float);
void func_00216778(void *, void *);
extern const Vector4Arg D_0043F610;
extern MapLoadState D_0038D610;
extern s32 D_00446770;

/* Makes a dying enemy drop its item under itself: the one its spawn record names when bit 62
   of that record's flags is set, otherwise the first or second item of its kind by their
   chances. Returns FALSE when nothing was dropped. */
s32 func_001C8CB0(Entity *entity) {
    f32 position[4];
    f32 rotation[4];
    Vector4Arg scale;
    f32 offset[4];
    s32 item;
    s32 priority;
    // match: one variable for the spawn record's item, both rolls and the result (separate ones move the item out of $3)
    s32 value;

    memset(rotation, 0, sizeof(rotation));
    rotation[3] = 1.0f;
    scale = D_0043F610;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 47)) {
        // match: the low 16 bits of the 64-bit field, copied before the test
        item = value = *(u16 *)&entity->spawn_info->unk38;
        if (value == 0xFFFF) {
            return FALSE;
        }
        priority = 1;
    } else {
        if (entity->params->drop_item == 0xFFFF) {
            return FALSE;
        }
        value = rand() * 100.0f * 4.656613e-10f;
        if (value < entity->params->drop_chance) {
            item = entity->params->drop_item;
            priority = 0;
        } else {
            if (entity->params->second_drop_item == 0xFFFF) {
                return FALSE;
            }
            value = rand() * 100.0f * 4.656613e-10f;
            if (value >= entity->params->second_drop_chance) {
                return FALSE;
            }
            item = entity->params->second_drop_item;
            priority = 0;
        }
    }
    position[0] = entity->model->position[0];
    position[1] = entity->model->position[1] - entity->height * 0.5f;
    position[2] = entity->model->position[2];
    rotation[1] = (s32)(rand() * 361.0f * 4.656613e-10f);
    rotation[1] = func_0019B838(rotation[1] * 0.017453292f);
    if (!func_001C0E40(2000.0f, position)) {
        return FALSE;
    }
    if (entity->group_id == D_00446770) {
        offset[0] = offset[1] = offset[2] = offset[3] = 0.0f;
    } else {
        func_00216778(offset, &D_0038D610.position);
    }
    value = func_001C8F18(entity->group_id, item, priority, position, COLLISION_RESULT->point[1], offset, rotation,
                          (f32 *)&scale, 0xFFFFFFFF);
    return value;
}
