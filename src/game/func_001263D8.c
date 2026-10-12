#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"

extern MapSlot D_0038CD20[];
extern const f32 D_004467F8[];

void func_00216778(void *, void *);
f32 func_0018A9F0(void *, void *);
f32 func_00194F48(f32 *, f32 *);

/* The map record the entity stands on: the one with the nearest floor under it, or failing that the one whose
   middle is nearest, among the records of the entity's map and then the other map. NULL when there is none. */
MapRecordBounds *func_001263D8(Entity *entity) {
    MapSlot *slots[2];
    f32 middle[4];
    f32 position[4];
    MapRecordBounds *nearest;
    MapRecordBounds *record;
    f32 nearest_distance;
    f32 distance;
    f32 reach;
    int map;
    int i;
    f32 height;

    nearest = NULL;
    nearest_distance = D_004467F8[0];
    reach = (entity->unk34 < entity->unk30) ? entity->unk30 : entity->unk34;
    func_00216778(position, entity->model->position);
    position[1] -= (reach < 75.0f) ? 75.0f : reach;
    slots[0] = &D_0038CD20[entity->group_id];
    slots[1] = &D_0038CD20[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->model->strips == NULL) {
                    continue;
                }
                if (entity->model->position[0] < record->lowest[0] - reach) {
                    continue;
                }
                if (record->highest[0] + reach < entity->model->position[0]) {
                    continue;
                }
                if (entity->model->position[2] < record->lowest[2] - reach) {
                    continue;
                }
                if (record->highest[2] + reach < entity->model->position[2]) {
                    continue;
                }
                height = func_0018A9F0(record, position);
                if (height != D_004467F8[0]) {
                    distance = height; // match: the copy to a second variable keeps the loop long enough that its end test is not hoisted
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        nearest = record;
                    }
                }
            }
        }
        if (nearest != NULL) {
            return nearest;
        }
    }
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (entity->model->position[0] < record->unk40[0] - reach) {
                    continue;
                }
                if (record->unk50[0] + reach < entity->model->position[0]) {
                    continue;
                }
                if (entity->model->position[1] < record->unk40[1] - reach) {
                    continue;
                }
                if (record->unk50[1] + reach < entity->model->position[1]) {
                    continue;
                }
                if (entity->model->position[2] < record->unk40[2] - reach) {
                    continue;
                }
                if (record->unk50[2] + reach < entity->model->position[2]) {
                    continue;
                }
                middle[0] = (record->unk40[0] + record->unk50[0]) * 0.5f;
                middle[1] = (record->unk40[1] + record->unk50[1]) * 0.5f;
                middle[2] = (record->unk40[2] + record->unk50[2]) * 0.5f;
                distance = ((f32 (*)(f32 *, f32 *))func_00194F48)(middle, entity->model->position); // match: returns the distance (other files declare it void)
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = record;
                }
            }
        }
        if (nearest != NULL) {
            return nearest;
        }
    }
    return NULL;
}
