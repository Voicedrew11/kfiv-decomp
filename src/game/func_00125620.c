#include "kf_entity.h"

extern int D_004467D8[2];
extern Entity D_003BEDF0[][0x80];
void func_00216C48(void *, void *, void *);

/* Moves every entity of one of the two entity arrays by an offset. */
void func_00125620(int array_index, f32 *offset) {
    Entity *entity;
    int i;
    int count;

    entity = D_003BEDF0[array_index];
    count = D_004467D8[array_index];
    for (i = 0; i < count; i++, entity++) {
        func_00216C48(entity->model->position, entity->model->position, offset);
        func_00216C48(entity->unk70, entity->unk70, offset);
        func_00216C48(entity->spawn_info, entity->spawn_info, offset);
    }
}
