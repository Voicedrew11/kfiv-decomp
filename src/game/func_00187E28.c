#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];

void Vector_Copy(void *, void *);
void sceVu0Normalize(float *, float *);
s32 func_00187A20(Entity *, ModelInstance *, f32 *, f32 *);

/* Pushes the entity out of the map pieces whose box it is in (its own map's, then the other
   loaded map's), remembers the piece it stands on in unk14 and updates its push (unkA4). */
void func_00187E28(Entity *entity) {
    f32 normal_sum[4];
    f32 position[4];
    MapSlot *slots[2];
    MapRecordBounds *record;
    f32 lift;
    s32 hit_floor;
    s32 map;
    s32 i;

    hit_floor = FALSE;
    lift = 0.0f;
    Vector_Copy(position, entity->model->position);
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    if (!((s32)(entity->flags >> 28) & 1)) {
        entity->flags &= ~(1L << 51);
        position[1] += 0.1f;
        lift = entity->unk48;
    }
    slots[0] = &D_0038CD20[entity->group_id];
    slots[1] = &D_0038CD20[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < slots[map]->header->record_count; i++, record++) {
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
                if (record->lowest[1] - entity->unk30 > entity->model->position[1]) {
                    continue;
                }
                if (record->highest[1] + entity->unk30 < entity->model->position[1]) {
                    continue;
                }
                if (record->lowest[2] - entity->unk30 > entity->model->position[2]) {
                    continue;
                }
                if (record->highest[2] + entity->unk30 < entity->model->position[2]) {
                    continue;
                }
                if (func_00187A20(entity, record->model, position, normal_sum)) {
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
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        normal_sum[1] += -1.0f;
        sceVu0Normalize(normal_sum, normal_sum);
        entity->unkA4[0] = normal_sum[0];
        entity->unkA4[1] = normal_sum[1];
        entity->unkA4[2] = normal_sum[2];
        return;
    }
    if (hit_floor) {
        ENTITY_FLAG_BITS(entity)->bit51 = FALSE;
        entity->model->position[1] = position[1];
        normal_sum[1] += -1.0f;
        sceVu0Normalize(normal_sum, normal_sum);
        entity->unkA4[0] = normal_sum[0];
        entity->unkA4[1] = normal_sum[1];
        entity->unkA4[2] = normal_sum[2];
    } else {
        ENTITY_FLAG_BITS(entity)->bit44 = FALSE;
        entity->model->position[1] = position[1] + lift;
    }
    entity->model->position[0] = position[0];
    entity->model->position[2] = position[2];
}
