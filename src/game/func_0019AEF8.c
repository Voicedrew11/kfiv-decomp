#include "kf_entity.h"
#include "kf_map.h"
#include "kf_model.h"

extern LinkedPoint *D_004470C8[2];

void *memset(void *, int, u32);

/* Collects the handles of the linked points whose reach touches the map record's box (at most 4, and at
   most 2 from each list), for the model the record is drawn with. */
void func_0019AEF8(MapDrawRecord *record, ModelInstance *model) {
    f32 unused[4]; // match: the frame has 16 unused bytes below the counters
    s32 taken[2];
    s32 *taken_start = taken; // match: memset through a pointer is a call; on the array itself it is inlined
    int i;
    LinkedPoint *node;
    f32 reach;

    memset(taken_start, 0, sizeof(taken));
    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = D_004470C8[i]; node != NULL; node = node->next) {
            if (taken[node->list_index] >= 2) {
                continue;
            }
            if (node->unk32 != -1 && node->unk32 != record->unk38) {
                continue;
            }
            reach = node->duration;
            if (!(0.0f < reach)) {
                continue;
            }
            if (record->unk40[0] - reach <= node->position.x && node->position.x <= record->unk50[0] + reach &&
                record->unk40[1] - reach <= node->position.y && node->position.y <= record->unk50[1] + reach &&
                record->unk40[2] - reach <= node->position.z && node->position.z <= record->unk50[2] + reach) {
                model->hit_handles[model->hit_count++] = node->handle;
                taken[node->list_index]++;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}
