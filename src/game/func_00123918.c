#include "kf_entity.h"

extern s64 D_003D6E40[];
extern int D_004467E8;

int func_001246A8(void);
int func_00124720(int);
void func_0010B758(ModelInstance *, int *, int *);
void func_001247F8(ModelInstance *);
void func_00124960(ModelInstance *);
void func_00124AF8(void);
// match: the definition takes an Obj, a struct local to func_0010CE08.c
s64 *func_0010CE08(ModelInstance *, s64 *);
void func_00108940(int, int *, ModelInstance *, int);
u32 *func_00116BD0(int, int);
void func_00124370(int);
void func_001251E0(void);

/* Gives an entity whose pool block is ready its work area and model; FALSE if it has to wait. */
int func_00123918(Entity *entity) {
    int data_address;
    int work_address;
    EntityPoolSlot *slot = &D_003D6DF0[entity->unkD3];
    EntityPoolData *data = (EntityPoolData *)slot->base_address;
    ModelInstance *model;
    int work_index;

    if (slot->head.fields.kind != ENTITY_POOL_READY) {
        return FALSE;
    }
    work_index = func_001246A8();
    if (work_index < 0) {
        return FALSE;
    }
    entity->unkD4 = work_index;
    work_address = func_00124720(work_index);
    data_address = slot->base_address + (int)data->model_offset;
    func_0010B758(ENTITY_MODEL(entity), &data_address, &work_address);
    model = ENTITY_MODEL(entity);
    model->draw = func_001247F8;
    model->unk1B0 = NULL;
    if (entity->spawn_info->type_id == 0x47) {
        model->unk100 |= 0x200;
        model->unk102 |= MODEL_FLAG_800;
        model->draw = func_00124960;
        model->unk1B0 = func_00124AF8;
        D_004467E8 = entity->spawn_info->type_id;
        work_address = (int)D_003D6E40;
        work_address = (int)func_0010CE08(model, (s64 *)work_address);
    }
    data_address = slot->base_address + (int)data->motion_offset;
    func_00108940(data_address, &work_address, ENTITY_MODEL(entity), 1);
    data_address = slot->base_address + data->unk0C;
    ENTITY_MODEL(entity)->unk194 = data_address;
    ENTITY_MODEL(entity)->unk190 = 0;
    ENTITY_MODEL(entity)->unk19C = (int)func_00116BD0(entity->group_id, 3);
    ENTITY_MODEL(entity)->unk1A0 = (int)func_00116BD0(entity->group_id, 5);
    entity->unkD2 = slot->unk7;
    func_00124370(entity->unkD3);
    ((void (*)(Entity *))func_001251E0)(entity); // match: declared without its argument elsewhere
    return TRUE;
}
