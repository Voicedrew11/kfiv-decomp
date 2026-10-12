#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
float func_00194F80(void *, void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00183240(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_0014C958(Entity *);

void func_0014CAF8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->spawn_info->unk40 == 3 && func_00194F80(entity->model->position, entity->spawn_info) > 180.0f) {
        ENTITY_FLAG_BITS(entity)->bit25 = 0;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                unkE0->unk6C = 0;
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                if (ENTITY_FLAG_BITS(entity)->bit32 && unkE0->unk6C == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_0014C958, unknown);
                    unkE0->unk6C = 1;
                } else {
                    func_00183240(entity, unknown);
                }
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
