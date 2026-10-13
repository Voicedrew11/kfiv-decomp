#include "kf_entity.h"
#include "kf_map_object.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_0017A880(Entity *, s32);
void func_0017B510(Entity *, s32);

void func_0017B380(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        unkE0->unk79 = (map_object->unk10 >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        unkE0->unk7A = (map_object->unk10 >> 17) & 1;
    }
    if (!unkE0->unk79 || !unkE0->unk7A) {
        func_0017B510(entity, unknown);
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_0017A880(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}
