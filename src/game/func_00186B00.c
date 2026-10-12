#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern f32 D_00446838[];

void func_00216778(void *, void *);
void sceVu0Normalize(float *, float *);
void func_00195100(f32 *, f32, f32);
s32 func_00186478(Entity *, ModelInstance *, f32 *, f32 *, f32 *);

/* Collides a walking entity with the map pieces whose box it is in (its own map's, then the
   other loaded map's): pushes it out of walls, updates its push (unkA4), and puts it on the
   floor, lets it fall, or stops it at a ledge. */
void func_00186B00(Entity *entity) {
    f32 normal_sum[4];
    f32 position[4];
    MapSlot *slots[2];
    void *unused[2];
    f32 floor_y;
    MapRecordBounds *record;
    f32 height;
    s32 hit_floor;
    s32 map;
    s32 i;

    hit_floor = FALSE;
    floor_y = D_00446838[0];
    height = entity->model->position[1];
    func_00216778(position, entity->model->position);
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    position[1] -= 0.1f;
    if (!((s32)(entity->flags >> 28) & 1)) {
        height += entity->unk48;
    }
    unused[0] = unused[1] = NULL;
    slots[0] = &D_0038CD20[entity->group_id];
    slots[1] = &D_0038CD20[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->unk08)->record_count; i++, record++) {
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->lowest[0] - entity->unk30 > entity->model->position[0]) {
                    continue;
                }
                if (record->highest[0] + entity->unk30 < entity->model->position[0]) {
                    continue;
                }
                if (record->lowest[1] - entity->unk34 > entity->model->position[1]) {
                    continue;
                }
                if (record->highest[1] + entity->unk34 < entity->model->position[1]) {
                    continue;
                }
                if (record->lowest[2] - entity->unk30 > entity->model->position[2]) {
                    continue;
                }
                if (record->highest[2] + entity->unk30 < entity->model->position[2]) {
                    continue;
                }
                if (func_00186478(entity, record->model, position, normal_sum, &floor_y)) {
                    entity->unk14 = record;
                    hit_floor = TRUE;
                }
            }
        }
        if (hit_floor) {
            break;
        }
    }
    normal_sum[0] += entity->unkA4[0];
    normal_sum[1] += entity->unkA4[1];
    normal_sum[2] += entity->unkA4[2];
    sceVu0Normalize(normal_sum, normal_sum);
    entity->unkA4[0] = normal_sum[0];
    entity->unkA4[1] = normal_sum[1];
    entity->unkA4[2] = normal_sum[2];
    if (ENTITY_FLAG_BITS(entity)->bit50) {
        ENTITY_FLAG_BITS(entity)->bit44 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit52 = FALSE;
        entity->unk48 = 10.0f;
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (ENTITY_FLAG_BITS(entity)->bit28) {
        ENTITY_FLAG_BITS(entity)->bit44 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit52 = FALSE;
        entity->unk48 = 10.0f;
        if (hit_floor && floor_y <= entity->model->position[1]) {
            ENTITY_FLAG_BITS(entity)->bit44 = TRUE;
            entity->model->position[1] = floor_y;
        }
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (floor_y <= entity->model->position[1]) {
        ENTITY_FLAG_BITS(entity)->bit44 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit52 = FALSE;
        func_00195100(&entity->model->position[1], floor_y, 5.0f);
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (floor_y <= height) {
        ENTITY_FLAG_BITS(entity)->bit44 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit52 = FALSE;
        entity->model->position[1] = floor_y;
    } else if (ENTITY_FLAG_BITS(entity)->bit44 && entity->params->unk42D != 0 && !(0.0f < entity->unk4C)
               && (entity->params->unk42D != 1 || !(floor_y <= entity->model->position[1] + entity->unk34))) {
        ENTITY_FLAG_BITS(entity)->bit44 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit52 = TRUE;
        entity->model->position[0] = entity->unk70[0];
        entity->model->position[1] = entity->unk70[1];
        entity->model->position[2] = entity->unk70[2];
        return;
    } else {
        ENTITY_FLAG_BITS(entity)->bit44 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit51 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit52 = FALSE;
        entity->model->position[1] = height;
    }
    entity->model->position[0] = position[0];
    entity->model->position[2] = position[2];
}
