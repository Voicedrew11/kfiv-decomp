#include "kf_entity.h"

extern ModelInstance D_003BE530[ENTITY_POOL_SLOT_COUNT];
extern char D_00474B90[ENTITY_POOL_SLOT_COUNT][0x2000];

void *memset(void *, int, u32);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010E8A0(ModelInstance *, s32);
void func_0010E6A8(ModelInstance *, s64, s64);

/* Clears a pool slot's model and work buffer, then builds the model from the slot's loaded data. */
void func_00124B00(int slot_index) {
    char *model_data;
    char *buffer;
    ModelInstance *model = &D_003BE530[slot_index];
    // match: never used, but it leaves the table address and the scaled index in registers before the calls
    EntityPoolSlot *unused_slot = &D_003D6DF0[slot_index];
    EntityPoolData *data;

    buffer = D_00474B90[slot_index];
    memset(model, 0, sizeof(ModelInstance));
    memset(buffer, 0, sizeof(D_00474B90[slot_index]));
    data = (EntityPoolData *)D_003D6DF0[slot_index].base_address;
    if (data->pool_model_offset != 0) {
        model_data = (char *)data + data->pool_model_offset;
        func_0010D018(model, &model_data, &buffer);
        func_0010E8A0(model, 0x5A0);
        func_0010E6A8(model, 0x20, 0x20);
    }
}
